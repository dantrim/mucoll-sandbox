# Simple muon gun

## Running

```verbatim
source /opt/ilcsoft/init_ilcsoft.sh
python gen/muon-gun-simple.py
ddsim --steeringFile muon_gun_sim_steer.py > sim.out 2>&1
Marlin ../steering_files/muon_gun_hits_reco_steer.xml > reco.out 2>&1
Marlin ../steering_files/muon_gun_tracks_reco_steer.xml > reco.out 2>&1
Marlin ../steering_files/lctuple.xml > ntuples.out 2>&1
```
