# Simple muon gun

## Running

First, initialize the Docker container with your run directory
using the [setup-env.sh script](../setup-env.sh) and initalize
the muon collider software:

```
$ mkdir run
$ cd run
$ source /path/to/mucoll-sandbox/setup-env.sh -c /path/to/mucoll-sandbox/mucoll_config
(docker) $ source /opt/ilcsoft/init_ilcsoft.sh
```

Then follow these steps to go through the process of generating a toy particle
gun sample, simulating it's interaction with the detector, reconstructing
the final state, and producing an output LCTuple ROOT file:

```
(docker) $ python /mucoll_config/generation/muon-gun-simple.py
(docker) $ ddsim --steeringFile=/mucoll_config/simulation/muon_gun_sim_steer.py --inputFiles="muonGun_gen.slcio" --outputFile="muonGun_sim.slcio" --numberOfEvents=100
(docker) $ Marlin --global.LCIOInputFiles="muonGun_sim.slcio" --Output_REC.LCIOOutputFile="muonGun_reco.slcio" --global.MaxRecordNumber=100 /mucoll_config/reconstruction/reco-mugun-pv-nocalo.xml
(docker) $ Marlin --global.LCIOInputFiles="muonGun_reco.000.slcio" --MyAIDAProcessor.FileName="muonGun_lctuple"
```

The above are the minimal steps to test the procedure of generating a muon gun 
sample, simulating the process evolution, and digitizing/reconstructing
the events in the muon collider detector.
