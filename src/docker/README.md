# Docker for sandbox development

You can develop the module in a sandbox envrionment. This sandbox environment contains all dependencies needed for compilation and testing the module.

## Prepare **.env**

Create local **.env** file with some constants of your choice.

```
URL=< [ optional ] The SRAM Server to request for introspection, defaults to https://sram.surf.nl>
TOKEN=< token value >
USERNAME=< sram user to be used during testing >
ENTITLED=< [ optional ] the SRAM entitlement to check if the user is member of, defaults to '*' >
```

## Start docker environment

```
docker-compose up
```

Now start bash session in container:

```
docker exec -ti sandbox bash
```

