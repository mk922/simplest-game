extends Polygon2D
func _ready() -> void:
	if randi() % 2 == 0:
		position.x = 576
func _process(_delta: float) -> void:
	if Input.is_action_just_released("ui_left"):
		if position.x == 0:
			if randi() % 2 == 0:
				position.x = 576
		else:
			get_tree().quit()
	if Input.is_action_just_released("ui_right"):
		if position.x == 576:
			if randi() % 2 == 0:
				position.x = 0
		else:
			get_tree().quit()
