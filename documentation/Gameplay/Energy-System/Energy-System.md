# Energy System
## Purpose:

The energy system is the primary method for balancing the player and their interactions
within the world.


## Overview:

There are two types of energy, active energy and latent energy.

<u>Active Energy:</u> The amount of energy that is available for immediate use.

<u>Latent Energy:</u> The amount of energy that is in long term storage. This energy needs
converted/transferred to Active Energy to be used.

There are three main attributes that entities/objects have regarding energy; storage, flow, and capacitance.
- <u>Latent Capacity:</u> is the total amount of Latent Energy an entity/object can have.
- <u>Flow:</u> is the rate in which Latenet Energy can be transfered to Active Energy.
- <u>Active Capacity:</u> is the amount of Active Energy an entity/object can have.

These three attributes balance each other for a specific entity/object as visualized by 
the energy attribute diagram below.

<div style="text-align: center;">

**Entity/Object Energy Attribute Balance Diagram**

\image latex /Energy-System/Energy_Triangle.png width=315px height=196px

</div>


## Portions of System that Need Elaboration:

- Player movement and spells
  - Spells are cast using energy and a slider (pre-decided?) of how much energy to use
- Movement, attacks, lifting all also take energy
- Tasks in general take energy
