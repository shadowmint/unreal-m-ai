# MAiRig

The 'Rig' system is a high level AI control system to use instead of the built in blackboard system.

Bluntly, the blackboard system is over-engineered for small purpose uses like game jams, and the
focus on visual editing severely restricts the amount of drop-in reuse you can do.

This system is much simpler and event based:

An AI pawn should have a component derived from `MAi_Rig_BaseRig` on it. This is a stateful component
that records the pawn state and offers a high level API to interact with it.

For example, move to point, attack target, move away from target, etc.

The AI controller should then be assigned a component derived from `MAI_Rig_BaseController` this is
responsible for processing the input events from the rig and trigger new actions.


