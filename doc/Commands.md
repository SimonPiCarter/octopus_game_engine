# Command available

## implemented

### Entity
- EntityMoveCommand
- EntityWaitCommand
- EntityAttackCommand : need to find new target

## to implement


### Entity
- EntityAttackMoveCommand : move to point issuing attack command to everything in a given range
- EntityPatrolCommand : attack move between two points
- EntityBuildCommand : move to a building and build it
- EntityHarvestCommand : move to a resource and harvest it, then go and drop it, repeat while no other command in queue

### Building
- BuildingTrainCommand
- BuildingResearchCommand
