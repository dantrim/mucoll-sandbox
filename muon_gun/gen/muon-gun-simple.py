#####################################
#
# simple script to create lcio files with single particle
# events - modify as needed
# @author F.Gaede, DESY
# @date 1/07/2014
#
# initialize environment:
#  export PYTHONPATH=${LCIO}/src/python:${ROOTSYS}/lib
#
#####################################
import math
import random
from array import array

# --- LCIO dependencies ---
from pyLCIO import UTIL, EVENT, IMPL, IO, IOIMPL

#---- number of events ----------------------
nevt = 10000

outfile = "muonGun_gen.slcio"

#--------------------------------------------


wrt = IOIMPL.LCFactory.getInstance().createLCWriter( )

wrt.open( outfile , EVENT.LCIO.WRITE_NEW ) 

random.seed()


#========== particle properties ===================

# particles per event
npart = 2

genstat  = 1

pt_min = 0.
pt_max = 100.

theta_min = 0.0
theta_max =2.5

pdg = 13

mass =  0.105658 
charge = -1.

decayLen = 1.e32

#=================================================


for j in range( 0, nevt ):

    col = IMPL.LCCollectionVec( EVENT.LCIO.MCPARTICLE ) 
    evt = IMPL.LCEventImpl() 

    evt.setEventNumber( j ) 

    evt.addCollection( col , "MCParticle" )

    if j % 100 == 0 :
        print(f" => Processing event # {j}")
    #print (j, "-----------------------------")
    
    for ipart in range( 0, npart ):
    
        pt = random.uniform(pt_min, pt_max)
        theta = random.uniform(theta_min, theta_max) 
        phi =  random.random() * math.pi * 2.

        p = pt/math.sin( theta )
        energy   = math.sqrt( mass*mass  + p * p ) 
        
        px = pt * math.cos( phi )
        py = pt * math.sin( phi )
        pz = p * math.cos( theta )

        momentum  = array('f',[ px, py, pz ] )  

        
        # --- endpoint
        
        epx = decayLen * math.cos( phi ) * math.sin( theta ) 
        epy = decayLen * math.sin( phi ) * math.sin( theta )
        epz = decayLen * math.cos( theta ) 

        endpoint = array('d',[ epx, epy, epz ] )  


        # --- production vertex

        vpx = 0.
        vpy = 0.
        vpz = 0.

        vertex = array('d',[ vpx, vpy, vpz ] )

        time = 0.


        # --- particle charge
        
        if ipart % 2 == 1:
            pdg = -pdg
            charge = -charge
        

        
#--------------- create MCParticle -------------------
        
        mcp = IMPL.MCParticleImpl() 

        mcp.setGeneratorStatus( genstat ) 
        mcp.setMass( mass )
        mcp.setPDG( pdg ) 
        mcp.setMomentum( momentum )
        mcp.setCharge( charge ) 
        mcp.setVertex( vertex )
        mcp.setTime( time )

        if( decayLen < 1.e9 ) :   # arbitrary ...
            mcp.setEndpoint( endpoint ) 

        print ("  ", ipart, pdg, charge, pt, phi, theta)
    

#-------------------------------------------------------


        col.addElement( mcp )

        
    wrt.writeEvent( evt ) 


wrt.close() 
