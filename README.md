![CodeQL](https://github.com/harrykodden/SRAM-Token-Service/workflows/CodeQL/badge.svg)
# Diagrams

### SRAM Authentication Generic Flow

This is the genric flow for services connected to **SRAM**. Users are redirected to **SRAM** for authentication. Via **SRAM** the user may select his home institute as their **Identity Provider**

```plantuml
!include assets/SRAM_Authentication.iuml
```

### SRAM - Token Introspection

```plantuml
!include assets/introspection.iuml
```

The **Service** validates the provided token using the Token Based Introspection endpoint of SRAM

```curl
curl --request POST \
  --url https://sram.surf.nl/api/tokens/introspect \
  --header 'Authorization: Bearer <service token>' \
  --header 'Content-Type: application/x-www-form-urlencoded' \
  --data token=<user token>
```

Examples:

* [Token Wrapping using Hashicorp Vault...](wrapping.md)
* [Service Authentication using a Wallet](wallet.md)
* [Service to Service Delegated Access](delegated_access.md)
* [Encrypted Mounting](hu.md)
