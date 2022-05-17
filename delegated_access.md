
# Delegated Access


## Option 1: Specific Trust Relationhip

Trust Relationship:

* Service A has knowledge of Service B "Service Bearer Token"


title Delegated access to Service

group Obtain user token for Service B
actor Researcher
Researcher -> Service_A: Authenticate SRAM portal
Researcher <-- Service_A: Redirect to SRAM
Researcher -> SRAM: Authenticate SRAM portal
Service_A <-- SRAM: OIDC Access Token
Researcher -> Service_A: Allow Service_B to authenticate on my behalf
Service_A --> SRAM: api/user_tokens/generate_token {Service_B Bearer Token + OIDC User Access Token }
Service_A <-- SRAM: User Token for Service_B
end
```


group Delegated Access
Service_A --> Service_B: [Username + User Token]
Service_B --> SRAM: PAM Validate [Username + Service B Bearer + User Token]
end
```

## Option 2: Generic flow to connect with a service.


group Process
Researcher -> ResearchCloud: User opens ResearchCloud
Researcher <-- ResearchCloud: redirect to SRAM for SRAM authentication
group SRAM Auth
Researcher -> SRAM: Authenticate with IDP
Researcher <-- SRAM: redirect to IDP
Researcher -> IDP: Authenticate, callback to SRAM
IDP --> SRAM : Success
Researcher <-- SRAM: Authenticated
end
group Connect with Service
Researcher -> ResearchCloud: Connect with a service...
ResearchCloud --> SRAM: GET /api/services
Researcher <-- ResearchCloud: Show services
Researcher -> ResearchCloud: Select service to connect with
ResearchCloud -> SRAM: /user_tokens/generate for that service
ResearchCloud <-- SRAM: User Token for that service
end
end
```

### Generic pattern authenticating to Service using a Token

The Client has:
* Username
* User Token

The Resource Server:
* Depends on an Access Server to validate Credentials

The Access Server:
* Runs Token Introspection agains SRAM to verify user token.



participant RP as "Client"
participant RS as "Resource Server"
participant AS as "Access Server"
group Client Authentication
RP --> RS: [Credentials: Username + User Token]
RS -> AS: Validating Credentials
AS -> SRAM: Do token introspection for [ Service Bearer Token + User Token ]
alt successful
AS <-- SRAM: details
AS <--> AS: Verify details
alt verify OK
RS <-- AS: Authenticated !
else verify not ok
RS <-- AS: No Access !
end
else failed to do introspection
RS <-- AS: No Access !
end
end
```

Example for iRODS:

Client = ResearchCloud
Resource Server = iRODS
Access Server = PAM Module


participant RP as "ResourceCloud"
participant RS as "iRODS"
participant AS as "PAM Module"
group Client Authentication
RP --> RS: [Credentials: Username + User Token]
RS -> AS: Validating Credentials
AS -> SRAM: Do token introspection for [ Service Bearer Token + User Token ]
alt successful
AS <-- SRAM: details
AS <--> AS: Verify details
alt verify OK
RS <-- AS: Authenticated !
else verify not ok
RS <-- AS: No Access !
end
else failed to do introspection
RS <-- AS: No Access !
end
end
```
