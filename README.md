# mucoll-sandbox

## Setup

### Configuration and steering files
The assumption is that you have some directory in which you have placed
all the steering and configuration files, for example [mucoll_config/](mucoll_config/),
that has the following structure:

```
mucoll_config/
 |__ generation/
 |__ simulation/
 |__ reconstruction/
```
with the corresponding steering and configuration files.

### Setting up your environment

In order to to perform muon collider studies, it is assumed that
you will be using the muon collider Docker containers.

You should setup your container by mounting both the directory
containing the configuration & steering files and
your work/run directory. For example, if you want to work
inside of a directory called `my_run_dir/` you can do and use
the configuration and steering files in [mucoll_config/](mucoll_config/),
you can do:

```
$ cd /path/to/my_run_dir
$ source /path/to/mucoll-sandbox/setup-env.sh -c /path/to/mucoll-sandbox/mucoll-config
(docker) $ ls
bin  dev  etc  home  lib  lib64  lost+found  media  mnt  mucoll_config  opt  proc  root  run  sbin  srv  sys  tmp  usr  var  workdir
```
which will put you inside of a Docker container with the directories `/mucoll_config`
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
