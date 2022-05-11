# Docker for sandbox development

You can develop the module in a sandbox envrionment. This sandbox environment contains all dependencies needed for compilation and testing tghe module.

## Prepare **.env**

Create local **.env** file with a TOKEN value for the SRAM Service bearer token.

```
TOKEN=< token value >
USERNAME=< sram user to be used during testing >
```

## Start docker environment

```
docker-compose up
```

Now start bash session in container:

```
docker exec -ti sandbox bash
```

