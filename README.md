
# Diagrams

### SRAM Authentication Generic Flow

This is the genric flow for services connected to **SRAM**. Users are redirected to **SRAM** for authentication. Via **SRAM** the user may select his home institute as their **Identity Provider**


![assets/SRAM_Authentication.iuml](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/HarryKodden/SRAM-Token-Service/main/assets/SRAM_Authentication.iuml)```

### Secrets Wallet Service

The wallet application allows authenticated users to pick up some secrets that are required to operate various services.


![assets/wallet.iuml](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/HarryKodden/SRAM-Token-Service/main/assets/wallet.iuml)```

### Token Wrapping for Impersonating users

Using token wrapping to handover access to user secrets to services in order to allow the service to access secrets on behalf of a certain secret owner.


![assets/wrapping.iuml](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/HarryKodden/SRAM-Token-Service/main/assets/wrapping.iuml)```

[Step by step explanation...](wrapping.md)

### SRAM - Service Impersonation


![assets/impersonation.iuml](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/HarryKodden/SRAM-Token-Service/main/assets/impersonation.iuml)```

### Privacy Sensitive Research Data Vault

___Under construction___

1. Store Privacy Senstive data in a Vault. Link back the reference to the privacy data in Files.


@startuml
Researcher -> Application
Application -> Files: Anonymous data
Application -> Vault: Privacy Sensitive data
Files <-- Vault: Link to secrets
@enduml
```

2. Store Privacy Senstive data encrypted in files. Store encryption key in Vault.


@startuml
Researcher -> Application
Application -> Files: Anonymous data
Application -> Cipher: Privacy Sensitive data: Create Cipher and encrypt data
Cipher -> Vault: Store decryption key
Cipher -> Files: Store encryped data + key reference
@enduml
```


### SRAM to Vault Synchronization

___Under construction___


![assets/sync_vault.iuml](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/HarryKodden/SRAM-Token-Service/main/assets/sync_vault.iuml)```

### SRAM to iRODS Synchronization

___Under construction___


![assets/sync_irods.iuml](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/HarryKodden/SRAM-Token-Service/main/assets/sync_irods.iuml)```

### SRAM to AzureAD Synchronization

___Under construction___


![assets/azure.iuml](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/HarryKodden/SRAM-Token-Service/main/assets/azure.iuml)```
