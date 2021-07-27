# mucoll-sandbox

## Setup

### Configuration and steering files
The assumption is that you have some directory in which you have placed
all the steering and configuration files, for example [mucoll_config/](mucoll_config/),
that has the following structure:

```
mucoll_config/
 |
 |__ generation/
 |__ simulation/
 |__ reconstruction/
```
with the corresponding steering and configuration files.

### Setting up your environment

In order to to perform muon collider studies, it is assumed that
you will be using the muon collider Docker containers.

You should setup your container by mounting both the directory
containing the configuration and steering files and optionally
the your work/run directory. For example, if you want to work
inside of a directory called `my_run_dir/` you can do and use
the configuration and steering files in [mucoll_config/](mucoll_config/),
you can do:

```
cd /path/to/my_run_dir
source /path/to/mucoll-sandbox/setup-env -c /path/to/mucoll-sandbox/mucoll-config
```
which will put you inside of a Docker container with the directories `/mucoll_config`
and `/workdir` being mounted. If the option `-w` is not provided to the `setup-env.sh` script, then `${PWD}` is mounted to `/workdir` in the Docker container.



```
python /mucoll_config/generation/muon-gun-simple.py
ddsim --steeringFile /mucoll_config/simulation/muon_gun_sim_steer.py --inputFiles="muonGun_gen.slcio" --outputFile="muonGun_sim.slcio"
Marlin --global.LCIOInputFiles="muonGun_sim.slcio" --Output_REC.LCIOOutputFile="muonGun_reco.slcio" /mucoll/reconstruction/reco-mugun-pv-nocalo.xml
Marlin --global.LCIOInputFiles="muonGun_reco.000.slcio" --MyAIDAProcessor.FileName="muonGun_lctuple" /mucoll/reconstruction/lctuple-mugun-pv.xml
