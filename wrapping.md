

# Token Wrapping

## Introduction

In this page a step by step walkthrough is explained hwo to use Token Wrapping.

The objective is to allow a Service to run a process on behalf of a user, using that user's password to authenticate to some resource.
In other words: The User delegates the use of his password to be used by a Service.

Here is the diagram on what we would like to achieve.


![assets/wrapping.iuml](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/HarryKodden/SRAM-Token-Service/main/assets/wrapping.iuml)```  


As you can see the diagram is divided in 3 major parts:

1. [Setup](#setup)
This is a one time action to setup a Service so that it can connect to Vault.
2. [Secret management](#secret-management-create--update)
This is an ongoing activity, users get secrets or updated secrets.
3. [Impersonating](#impersonating)
This takes place whenever a Service needs to process a task unattended on behalf of a user. The service needs access to certain user specific secrets.

The 3 parts are explained in more detail.

## Setup

The setup actions need to take place:

1. [Enable AppRole](#enable-approle)
   This is a 1 time action.
2. [Create an AppRole for a Service](#create-an-approle-for-service-my-service)
   This is required for every service that needs to be able to impersonate
3. [Get Credentials for that Approle](#create-credentials-for-my-service-to-connect-to-vault)
Create unprivileged credentials for the service to connect to Vault with AppRole
1. [Handover credentials to Service](#handover-credentials-to-my-service)
One time per service: Pass the credentials for that service

### Enable AppRole

First we need to make sure that the AppRole feature is an enabled functionaly in Vault.
This is a one-time activation and can be achieved by this command:

```bash

curl --request POST \
  --url https://vault.example.org/v1/sys/auth/approle \
  --header 'Content-Type: application/json' \
  --header 'X-Vault-Token: <your vault token>' \
  --data '{
 "type": "approle"
  }'
```

### Create an AppRole for Service **my-service**

Now, create a named approle entity that and provide it with credentials to allow this 'app' connect with Vault.
For this moment we create a bare minimum approle entity. Vault allows for much more app characteristics that provide much more control over the connection between approles and Vault. Refer: https://www.vaultproject.io/api/auth/approle#parameters

```bash

curl --request POST \
  --url https://vault.example.org/v1/auth/approle/role/my-service \
  --header 'Content-Type: application/json' \
  --header 'X-Vault-Token: <your vault token>' \
  --data '{
 "policies": "default"
  }'
```

### Create Credentials for **my-service** to connect to Vault

An Approle needs credentials to perform a Login to Vault. 
The credentials to do so are **role_id** and **secret_id**.

**Get Role ID**

The **role_id** can be obtained via:

```bash

curl --request GET \
  --url https://vault.example.org/v1/auth/approle/role/my-service/role-id \
  --header 'X-Vault-Token: <your vault token>'
```

Vault returns data like:

```json

{
  ...
  "data": {
    "role_id": "70389ba5-dcea-5bef-5919-fd4e2bf3e4df"
  },
  ...
}
```

**Get Secret ID**

The **secret_id** can be obtained via:

```bash

curl --request POST \
  --url https://vault.example.org/v1/auth/approle/role/my-service/secret-id \
  --header 'X-Vault-Token: <your vault token>'
```

Vault returns data like:

```json

{
  ...
  "data": {
    "secret_id": "16d0f573-bb35-5b03-d57f-f81ccfdba553"
    ...
  },
  ...
}
```

### Handover credentials to **my-service**

Pass in a secured way the credentials payload to **my-service**

```json

{
  "role_id": "70389ba5-dcea-5bef-5919-fd4e2bf3e4df",
  "secret_id": "16d0f573-bb35-5b03-d57f-f81ccfdba553"
}
```

## Secret management (Create / Update)

This is ongoing activity, new users get secrets, existing users get updated secrets, users get deleted, etc.
What is important here, is that we create a policy one time per user per secret. This specific policy allows READ permission to this specific secret of this specific user.
At this stage nobody (except Vault Root) has access this secret.

### Create Secret

```bash

curl --request PUT \
  --url https://vault.example.org/v1/secret/data/services/my-service/my-user \
  --header 'Content-Type: application/json' \
  --header 'X-Vault-Token: <your vault token>' \
  --data '{
    "data": {
    "password":"mypassword"
    }
  }'
```

### Create Policy

We create a unique policy that only allows 'read' operation on a single identified secret.

```bash

curl --request PUT \
  --url https://vault.example.org/v1/sys/policy/my-service-my-user-policy \
  --header 'Content-Type: application/json' \
  --header 'X-Vault-Token: <your vault token>' \
  --data '{ 
    "policy": "path \"/secret/data/services/my-service/my-user\" { capabilities = [\"read\"] }"
  }'
```

## Impersonating

Now the exciting stuff, how can we make **my-service** to read the secret, so that it can execute on behalf of the user ?

### Wrap token

First we need to ask Vault to prepare a "wrapped" token, with the policy that we would like to handover to the service.

```bash

curl --request POST \
  --url https://vault.example.org/v1/auth/token/create \
  --header 'Content-Type: application/json' \
  --header 'X-Vault-Token: <your vault token>' \
  --header 'X-Vault-Wrap-TTL: 120' \
  --data '{
    "policies":["my-service-my-user-policy"]
  }'
```

The data we get from this request looks something like this:

```json

{
  ...
  "wrap_info": {
    "token": "s.NMHdfWq88oAZwXs5gHxhiHzm",
    "accessor": "GOIqcNlxxbfoKepHcbd4dOds",
    "ttl": 120,
    "creation_time": "2021-11-28T22:21:04.008207505Z",
    "creation_path": "auth/token/create",
    "wrapped_accessor": "42bCxntvmTTzcdIGCsK4A1KU"
  },
  ...
}
```

We now take out the **token** form message and pass that to the service is a trusted way:

```json

{
    "token": "s.NMHdfWq88oAZwXs5gHxhiHzm"
}
```

For the Service in order to consume this token, it first has to authenticate to Vault using his (unprivileged) AppRole credentials:

```bash
curl --request POST \
  --url https://vault.example.org/v1/auth/approle/login \
  --header 'Content-Type: application/json' \
  --data '{
     "role_id": "70389ba5-dcea-5bef-5919-fd4e2bf3e4df",
    "secret_id": "16d0f573-bb35-5b03-d57f-f81ccfdba553"
  }'
```

This delivers the **client_token** to connect to Vault (notice that the given policies to this token is limited to 'default'):

```json

{
  ...
  "auth": {
    "client_token": "s.H03l9iVIqeOAtNBoGCFsCduj"
    ...
    "policies": [
      "default"
    ],
  },
  ...
}
```

Now the Service requests Vault to unwrap the received wrapped token using this **client_token**:

```bash

curl --request POST \
  --url https://vault.example.org/v1/sys/wrapping/unwrap \
  --header 'Content-Type: application/json' \
  --header 'X-Vault-Token: s.H03l9iVIqeOAtNBoGCFsCduj' \
  --data '{
  "token": "s.NMHdfWq88oAZwXs5gHxhiHzm"
}'
```

Vault returns (new) **client_token**, with extra policies attached to it:

```json

{
  ...
  "auth": {
    "client_token": "s.PTDwZPrMu7dubmU6sims41Xc",
    ...
    "policies": [
      "default",
      "my-service-my-user-policy"
    ],
    ...
  }
}
```


With this new **client_token**, the Service reads the secret:

```bash

curl --request GET \
  --url https://vault.example.org/v1/secret/data/services/my-service/my-user \
  --header 'X-Vault-Token: s.PTDwZPrMu7dubmU6sims41Xc'
```

The data return looks like:

```json

{
  ...
  "data": {
    "data": {
      "password": "mypassword"
    },
    ...
  },
  ...
}
```
