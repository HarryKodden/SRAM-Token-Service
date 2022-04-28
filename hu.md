### Privacy Sensitive Research Data Vault

___Under construction___

1. Store Privacy Senstive data in a Vault. Link back the reference to the privacy data in Files.

```plantuml
@startuml
Researcher -> Application
Application -> Files: Anonymous data
Application -> EncryptedDrive: Privacy Sensitive data
@enduml
```

2. Store Privacy Senstive data encrypted in files. Store encryption key in Vault.

```plantuml
@startuml
Researcher -> Application
Application -> Files: Anonymous data
Application -> Cipher: Privacy Sensitive data: Create Cipher and encrypt data
Cipher -> Vault: Store decryption key
Cipher -> Files: Store encryped data + key reference
@enduml
```

Sleutelbestand staat in de kluis