# Command available

## implemented

### Entity
- EntityMoveCommand
- EntityWaitCommand
- EntityAttackCommand
- EntityAttackMoveCommand : move to point issuing attack command to everything in a given range

## to implement


### Entity
- EntityPatrolCommand : attack move between two points
- EntityBuildCommand : move to a building and build it
- EntityHarvestCommand : move to a resource and harvest it, then go and drop it, repeat while no other command in queue

### Building
- BuildingTrainCommand
- BuildingResearchCommand
