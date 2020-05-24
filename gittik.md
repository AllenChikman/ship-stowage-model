![build status](https://travis-ci.com/OrrBin/thoughts.svg?branch=master)
[![codecov](https://codecov.io/gh/OrrBin/thoughts/branch/master/graph/badge.svg)](https://codecov.io/gh/OrrBin/thoughts)
[![Documentation Status](https://readthedocs.org/projects/thoughts/badge/?version=latest)](https://thoughts.readthedocs.io/en/latest/?badge=latest)

# Thoughts

Advanced System Design project : Brain computer interface

## Overview
Thoughts is a project that aims to simulate Brain computer interface
 which captures, parse, store and expose  user stats, such as feelings, pose and what he sees.
The project is built in microservices architecture, and was built while taking 
into consideration scaling and flexibility.  

Each microservice is implemented as a python package, where `utils`, `core`, `message_queues`, `persistence.databases` 
are shared packages between the services. 

## Installation
1. Clone the repository and enter it:

    ```
    $ git clone git@github.com:OrrBin/thoughts.git
    ...
    $ cd thoughts/
    ```

2. Run the installation script and activate the virtual environment:

    ```
    $ ./scripts/install.sh
    ...
    $ source .env/bin/activate
    [thoughts] $ # you're good to go!
    ```
   
3. Install docker: https://docs.docker.com/engine/install/ubuntu/ 

4. Install docker-compose:https://docs.docker.com/compose/install/  

5. To start up the whole system at once:
    
    ```
    $ ./run_pipeline.sh
    ...
    ```
6. More on running the microservices in `Microservices` section

7. See The repository https://github.com/OrrBin/thoughts-website for GUI

## General notes
1. If running with default values, it is expected that the user that runs the services would have 
permissions to create, read and write `/var/data/thoughts`

2. When running `docker-compose`, currently the gui is accessing the api through the host machine via ip `172.19.0.1`.  
   This is because for some reason accessing through the `api` name doesn't work, only for this service. 

## Starting the whole shabang
To start all the microservices at once, one easy command is provided: 
    
    $ ./run_pipeline.sh

This command depends on docker-compose so make sure it is installed.
The file `docker-compose.yml` defines the composition, and it uses `thoughts.env`
to define global environment variables
 
##### NOTE
For some reason the gui fails to communicate with the api using `api` host name,
So i use a hack, and go through the host machine, see in `thoughts.env` the `API_URL` comment for more information

## Microservices
### server
Listens to snapshots update requests, sent by the client.
The server is responsible for converting the input from the client to the format that
is used to communicate between the microservices.
The server saves the large data objects, like images, to disk and sends lightweight data to 
a message queue to be consumed by `parser`s

The server can receive root folder to save the data to. if not given the default is `/var/data/thoughts`.
The data would be saved in sub-folder `data` 

#### Staring the server
    
    $ python -m thoughts.server run-server # With default values
    
    ----------------------------------------------------------------------------------
    
    $ python -m thoughts.server run-server -h 0.0.0.0 -p 8003 -d ~/thoughts # With custom values

#### Staring the server using docker (Example)
When running with docker the server relies on environment variable `MQ_URL` to connect to a message queue.
default value is `rabbitmq://127.0.0.1:5672`.

When running using docker the data is saved to the default folder `/var/data/thoughts` in the container.

    $ docker run -p 8000:8000  --name server --network my-net -v ~/thoughts:/var/data/thoughts 
    -e MQ_URL=rabbitmq://rabit:5672 thoughts-server # using custom message queue url where rabit is the name of 
                                                    # A container runnig RabbitMQ service 
    
### saver
Responsible for saving parsed data to data stores.
Currently supported data stores are: MongoDB

#### Staring the saver

    $ python -m thoughts.persistence run-saver

#### Staring the saver using docker (Example)
When running with docker the api relies on environment variable `DB_URL` to connect to a database.
default value is `mongodb://127.0.0.1:27017`.

Also relies on environment variable `MQ_URL` to connect to a message queue.
default value is `rabbitmq://127.0.0.1:5672`.

    $ docker run  --name saver --network my-net -v ~/thoughts:/var/data/thoughts
     -e DB_URL=mongodb://mongo:27017  
     -e MQ_URL=rabbitmq://rabit:5672 thoughts-saver # Example using custom url, where rabit is the
                                                    # name of a docker container, that runs RabitMQ service
    
### parser
Each parser is responsible to parse specific part of the snapshot,
then publish it to message queue to be consumed by a `saver`.
Current implemented parsers: `feelings`, `pose`, `color image`, `depth image`

Parsers can receive root folder to save the data to. if not given the default is `/var/data/thoughts`.
Parser should save their data sub-folder of the root data.
For example `images` for color image or `heatmap` for depth image  

#### Staring the parser
 
    $ python -m thoughts.parsers run-parsers #run all parsers

    ----------------------------------------------------------------------------------
    
    $ python -m thoughts.parsers run-parsers -p color_image #run only color image parser

    ----------------------------------------------------------------------------------
    
    $ python -m thoughts.parsers run_parser color_image  #run color_image parser
    

#### Staring the parser using docker (Examples)
When running with docker the parser relies on environment variable `PARSER` to select which parser to start.
default value is `all` in which case all parsers would be started
 
Also relies on environment variable `MQ_URL` to connect to a message queue.
default value is `rabbitmq://127.0.0.1:5672`.

When running using docker the data is saved to the default folder `/var/data/thoughts` in the container.

    $ sudo docker run  --name parsers --network my-net -v ~/thoughts:/var/data/thoughts
     -e MQ_URL=rabbitmq://rabit:5672 thoughts-parsers   # starting all parsers as one container, default
                                                        # using custom message queue url where rabit is the name of 
                                                        # A container running RabbitMQ service 
    
    ----------------------------------------------------------------------------------
    
    $ sudo docker run  --name parsers --network my-net -v ~/thoughts:/var/data/thoughts
     -e PARSER=all -e MQ_URL=rabbitmq://rabit:5672 thoughts-parsers #starting all parsers as one container
     
     ----------------------------------------------------------------------------------
     
     $ docker run  --name parsers --network my-net -v ~/thoughts:/var/data/thoughts
     -e PARSER=pose -e MQ_URL=rabbitmq://rabit:5672 thoughts-parsers # starting only pose parser as one container

    
### api
Exposes REST api that exposes the data stored in the data store.

#### Staring the api
    
    $ python -m thoughts.api run-server -d mongodb://128.0.0.5:27017 #example using  custom url
   
   
#### Staring the api using docker (Example)
When running with docker the api relies on environment variable `DB_URL` to connect to a database.
default value is `mongodb://127.0.0.1:27017`.

    $ docker run -p 5000:5000  --name api --network my-net -v ~/thoughts:/var/data/thoughts
     -e DB_URL=mongodb://mongo:27017 thoughts-api   # Example using custom url, where mongo is the
                                                    # name of a docker container, that runs MongoDB service 

### client
Python module that exposes one function `upload_sample` that gets 
sample file, and uploads all snapshots from it to the `server`
 
#### Staring the client
    python -m thoughts.client upload-sample
    
    
### cli
Python module that consumes the main functions of the `api`

### gui
Simple webservice that serves webapp. 
Please see https://github.com/OrrBin/thoughts-website repository for more details the webapp.
Note that the file `thoughts/gui/static/env.js` is configuration file for the webapp,
and the variable `window.__env.apiUrl` defined the default api url.
Update to this file are applied to the webapp without the need to restart the webservice.

#### Staring the gui 
see variable `window.__env.apiUrl` in file `thoughts/gui/static/env.js` for current api url.
If `-a` or is passed, then before starting the webservice, the configuration file `thoughts/gui/static/env.js`
is updated with the provided api url    

    $ python -m thoughts.gui run-server -a http://128.0.0.5:5000 #example using env var to pass custom url
   
   

#### Staring the gui using docker (Example)
When running via docker, the gui relies on environment variable `API_URL` to connect to a api.
see variable `window.__env.apiUrl` in file `thoughts/gui/static/env.js` for current api url.
If `API_URL` is defined, then before starting the webservice, the configuration file `thoughts/gui/static/env.js`
is updated with the provided api url

    $ sudo docker run -p 5555:5555  --name gui --network my-net
     -e API_URL=http://api:5000 thoughts-gui # Example using custom url, where api is the
                                             # name of a docker container named api, that runs API service
                                             

 
## Extend the project
This project is meant to be easy to extend. here are some useful tips on how you can easily extend it

### Add another type of database driver
To add support for another database driver following steps are needed:
    
    1. add a file, with name that end with db.py, for example postgresdb.py
    2. implement the driver and add prefix attribute with the specific
       database type, for exmpale prefix = postgres
    3. To see the api the driver should implement see mongodb.py
    4. The driver should have a constructor accepting: host, port
 
 ### Add another type of message queue driver
 To add support for another message queue following steps are needed:
    
    1. file, with name that end with mq.py
    2. implement the driver and add prefix attribute with the specific message queue type
    3. The api the handler must implement is:
        publish(self, topic, message)
        consume(self, topic, handler)
    4. The driver should have a constructor accepting: host, port
    
 ### Add another type of parser
 To add a new type of snapshot parser:
    
    1. Add a function in the thoughts/parsers folder (at some file), reciving sanpshot bytes, root data folder
    2. implement the parser and add identifier attribute with the specific parser type, for example anger
    3. The parser must return dict with one element: {identifier: value}
       Where value is the parsed value, and identifier is the parser identifier, for exmple {anger : 0.5}