
# Delegated Access


## Option 1: Specific Trust Relationhip

Trust Relationship:

* Service A has knowledge of Service B "Service Bearer Token"


![assets/delegated_access/option_1/option_1a.iuml](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/HarryKodden/SRAM-Token-Service/main/assets/delegated_access/option_1/option_1a.iuml)```


![assets/delegated_access/option_1/option_1b.iuml](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/HarryKodden/SRAM-Token-Service/main/assets/delegated_access/option_1/option_1b.iuml)```

## Option 2: Generic flow to connect with a service.


![assets/delegated_access/option_2/option_2a.iuml](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/HarryKodden/SRAM-Token-Service/main/assets/delegated_access/option_2/option_2a.iuml)```



### Generic pattern authenticating to Service using a Token

The Client (OAuth Relying Party) has:
* Username
* User Token

The Resource Owner (OAuth):
* Depends on an Access Server to validate Credentials

The Access Server:
* Runs Token Introspection agains SRAM to verify user token.



![assets/delegated_access/option_2/option_2b.iuml](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/HarryKodden/SRAM-Token-Service/main/assets/delegated_access/option_2/option_2b.iuml)```


Example for iRODS:

* Client = ResearchCloud
* Resource Server = iRODS
* Access Server = PAM Module


![assets/delegated_access/option_2/option_2c.iuml](https://www.plantuml.com/plantuml/proxy?cache=no&src=https://raw.github.com/HarryKodden/SRAM-Token-Service/main/assets/delegated_access/option_2/option_2c.iuml)```
