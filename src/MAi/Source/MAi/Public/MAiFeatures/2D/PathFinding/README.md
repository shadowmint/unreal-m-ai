# Pathfinding

How to use this:

- Add an MAi_2D_PathController to the scene.
- Add any number of objects which have MAi_2D_PathNode elments on them.
- Add splines to the same objects. 

Like this:

```
- Level
-- MAi_2D_PathController
-- Node1
---- Spline1
---- Spline2
-- Node2
---- Spline1
...
```

Every `Node` needs to specify what path group it is a member of.

When a pawn is required to move to a specific location, use:

```
MAi_2D_PathController *Controller = MAi_2D_PathController::GetLevelController();
Controller->MoveTo(Pawn, Destination, PathGroup);
```

If you need a callback for arrival, use:

```
Pawn->Movement->OnMoved
```

To bind a callback.

If you need a task for some reason, use:

```
Controller->MoveToTask();
```
