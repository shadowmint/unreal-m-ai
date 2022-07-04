# Movement

Assign movement and animation to the Pawn, so that it has both, like this:

```
- Pawn
-- MAi_2D_Movement
-- MAi_2D_MovementAnimation
```

Configure the movement object, and then you can add call `NavigateTo` on it to move on a marked path.

The path requires a `PathController` on the level, see the `PathFinding` folder.