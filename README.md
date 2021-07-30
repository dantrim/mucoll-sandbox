# mucoll-sandbox

## Cloning the `mucoll-sandbox` repository
You must perform a recursive clone of this repository if you wish to also
pull all of the configuration files from the [ProductionConfig](ProductionConfig/)
directory:

```
git clone --recursive https://github.com/dantrim/mucoll-sandbox.git
```

## Setup

### Configuration and steering files
Place all configuration and steering files under [ProductionConfig](ProductionConfig),
under your specific sub-directories for your studies.
The sub-directories within [ProductionConfig](ProductionConfig)
are further broken down in terms of event generation ([ProductionConfig/evtgen](ProductionConfig/evtgen)),
detector simluation ([ProductionConfig/simulation](ProductionConfig/simulation)),
and reconstruction ([ProductionConfig/reconstruction](ProductionConfig/reconstruction)):
```
ProductionConfig/
|-- evtgen/
|-- simulation/
|-- reconstruction/
```

### Setting up your environment

In order to perform muon collider studies, it is assumed that
you will be using the muon collider Docker containers.
See [Pulling Muon Collider Docker Images](#pulling-muon-collider-docker-images)
if you do not have them already on your machine.

You should setup your container by mounting both the directory
containing the configuration & steering files and
your work/run directory. For example, if you want to work
inside of a directory called `my_run_dir/` you can do and use
the configuration and steering files in [ProductionConfig/](ProductionConfig/),
you can do:

```
$ cd /path/to/my_run_dir
$ source /path/to/mucoll-sandbox/setup-env.sh -c /path/to/mucoll-sandbox/ProductionConfig
(docker) $ ls
bin  dev  etc  home  lib  lib64  lost+found  media  mnt  ProductionConfig  opt  proc  root  run  sbin  srv  sys  tmp  usr  var  workdir
```
which will put you inside of a Docker container with the directories `/ProductionConfig`
and `/workdir` being mounted.

If the option `-w` is not provided to the `setup-env.sh` script, then whichever directory you are in
when you called `setup-env.sh` (i.e. `${PWD}`) will be mounted to `/workdir` in the Docker container.
Otherwise, the local directory specified by `-w` will be mounted to `/workdir` inside of the Docker container.

### Running muon collider software

Once you are inside of your Docker environment (see [Setting up your environment](#setting-up-your-environment))
you need to setup the muon collider software:

```
(docker) $ source /opt/ilcsoft/init_ilcsoft.sh
```
at which point you should be able to run
```
(docker) $ Marlin -h
(docker) $ ddsim -h
```
without failures.

## Pulling Muon Collider Docker Images

You must have Docker installed on your host machine.

Then do:
```
docker pull infnpd/mucoll-ilc-framework:1.5-centos8
```
