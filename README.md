# SMO-TAS-Exlaunch

A TASing mod built for Super Mario Odyssey.

## Features
- Semi-robust debug menu with easy customizability
- Ability to show all HitSensors of actors in the scene, with tons of
toggleable options. 
- Rough setup for TASing

## TAS
Because of the ability to control the game's controller inputs directly, we can
finally add support for motion controls. This means that scripts will now be 
longer due to the amount of variability in controller inputs per frame. There are 
also plans to create a script converter from the old format to the new format.

### Format Changes

**THIS IS ALL SUBJECT TO CHANGE**

First and foremost, it's important to know that scripts will be read by the game
if they are in a binary format. This makes it very easy to separate the different
kinds of inputs we have.

Scripts contain useful information on how they should be run at the very beginning.
All scripts must contain "BOOB" at the very beginning as a sort of indicator to the game
that this is a script that can be run and not just some strange file. 
Then, the metadata line contains the following:
- ChangeStangeName
- ChangeStageId
- Scenario Number
- The number of frame lines the script contains (not the highest number)
- If the script is meant for 2-Player mode

With the change of making 2 Player mode read from the same script, 
each player line will need to be next to each other. As such, the format becomes as follows:

`<frameNo> <isPlayerTwo> <buttons> <lStick> <rStick> <leftGyro> <leftAngularV> <rightGyro> <rightAngularV>`

where
- `<frameNo>` is a `u32`
- `<isPlayerTwo>` is a `bool`
- `<buttons>` is a `u32`
- `<lStick>` is a pair of `float`s (`Vector2f`)
- `<rStick>` is a `Vector2f`
- `<leftAccel>` is a trio of `float`s (`Vector3f`)
- `<rightAccel>` is a `Vector3f`
- `<leftGyro>` is a `Matrix33f`
- `<leftAngularV>` is a `Vector3f`
- `<rightGyro>` is a `Matrix33f`
- `<rightAngularV>` is a `Vector3f`


## Notes

- Currently only works on v1.0. 
- Motion Control TASing is not yet working as intended. 

# Credits

- [exlaunch](https://github.com/shadowninja108/exlaunch/)
- [exlaunch-cmake](https://github.com/EngineLessCC/exlaunch-cmake/)
- [BDSP](https://github.com/Martmists-GH/BDSP)
- [Sanae](https://github.com/Sanae6)
- [Atmosphère](https://github.com/Atmosphere-NX/Atmosphere)
- [oss-rtld](https://github.com/Thog/oss-rtld)
