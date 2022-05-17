# Delegated Access


## Option 1: Specific Trust Relationhip

Trust Relationship:

* Service A has knowledge of Service B "Service Bearer Token"

```plantuml
!include assets/delegated_access/option_1/option_1a.iuml
```

```plantuml
!include assets/delegated_access/option_1/option_1b.iuml
```

## Option 2: Generic flow to connect with a service.

```plantuml
!include assets/delegated_access/option_2/option_2a.iuml
```



### Generic pattern authenticating to Service using a Token

The Client has:
* Username
* User Token

The Resource Server:
* Depends on an Access Server to validate Credentials

The Access Server:
* Runs Token Introspection agains SRAM to verify user token.


```plantuml
!include assets/delegated_access/option_2/option_2b.iuml
```


Example for iRODS:

Client = ResearchCloud
Resource Server = iRODS
Access Server = PAM Module

```plantuml
!include assets/delegated_access/option_2/option_2c.iuml
```
