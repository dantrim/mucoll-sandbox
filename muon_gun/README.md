# Simple muon gun

## Running

These are minimal steps to test the procedure of generating a muon gun 
sample, simulating the process evolution, and digitizing/reconstructing
the events in the muon collider detector.

The reconstruction goes so far as to run the Pandora Particle Flow Algorithm
(PandoraPFA) in order to then run the primary vertex finding algorithm
from within the LFVIPlus.

```verbatim
source /opt/ilcsoft/init_ilcsoft.sh
mkdir run
cd run
python ../gen/muon-gun-simple.py
ddsim --steeringFile ../sim/muon_gun_sim_steer.py > sim.out 2>&1
Marlin ../reco/reco-mugun-pv.xml > reco.out 2>&1
Marlin ../reco/lctuple-mugun-pv.xml > ntuple.out 2>&1
```
