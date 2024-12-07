---
title: M3-CRUD API
draft: false
tags:
  - crud-api
---
## APIs

There are several types of APIs. 

APIs are used to interact with a database, so that we can specify the requested table and requested row within our API query, and then use HTTP method to perform the operation needed. 

Let's say there is `api.php` and we want to update the `city` table in the database, and the row we will be updating has a city name of `seoul`, then the URL would look something like this:

```bash
curl -X PUT http://<SERVER_IP>:<PORT>/api.php/city/london ...SNIP...
```

## CRUD

In general, APIs perform 4 main operations on the requested database entity:

|Operation|HTTP Method|Description|
|---|---|---|
|`Create`|`POST`|Adds the specified data to the database table|
|`Read`|`GET`|Reads the specified entity from the database table|
|`Update`|`PUT`|Updates the data of the specified database table|
|`Delete`|`DELETE`|Removes the specified row from the database table|

Above four operations are mainly linked to `CRUD APIs`, but the same principal is used in `REST APIs` as well. 

Obviously, not all APIs work in the same way and user access control limits what actions we can perform and what results we can see. 

## Read

We can interact with an API to read data as such:

```shell-session
jadu101@htb[/htb]$ curl http://<SERVER_IP>:<PORT>/api.php/city/london

[{"city_name":"London","country_name":"(UK)"}]
```

To have the json result properly formatted in JSON, we can use `jq` utility:

```shell-session
jadu101@htb[/htb]$ curl -s http://<SERVER_IP>:<PORT>/api.php/city/london | jq

[
  {
    "city_name": "London",
    "country_name": "(UK)"
  }
]
```


## Create

We can use `HTTP POST`  to add a new entry. 

```shell-session
jadu101@htb[/htb]$ curl -X POST http://<SERVER_IP>:<PORT>/api.php/city/ -d '{"city_name":"HTB_City", "country_name":"HTB"}' -H 'Content-Type: application/json'
```

## Update

`PUT` is used to update API entries and modify their details. It similar to `POST` but it is different that we have to specify the name of the entity we want to edit in the URL. 

`DELETE` is used to remove a specific entity.


```shell-session
jadu101@htb[/htb]$ curl -X PUT http://<SERVER_IP>:<PORT>/api.php/city/london -d '{"city_name":"New_HTB_City", "country_name":"HTB"}' -H 'Content-Type: application/json'
```

Above command replaces `london` with `New_HTB_City`. 

## DELETE

`DELETE` is used to delete the entry.

```shell-session
jadu101@htb[/htb]$ curl -X DELETE http://<SERVER_IP>:<PORT>/api.php/city/New_HTB_City
```

