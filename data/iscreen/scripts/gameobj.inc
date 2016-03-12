	Object $GAME_OBJ_NAME {
		$GAME_OBJECT
		#define OBJ1	"SELECTED_OBJ"
		#define OBJ2	$GAME_OBJ_NAME
		Event {
			if_not_selected
			key $VK_LBUTTON
			key $VK_MOVE

			$EV_CHANGE_OBJ
		}

		#define OBJ1	$GAME_OBJ_NAME
		#define OBJ2	$GAME_OBJ_NEXT
		Event {
			if_selected
			key $VK_DOWN
			key $VK_TAB

			$EV_CHANGE_OBJ
		}

		#define OBJ1	$GAME_OBJ_NAME
		#define OBJ2	$GAME_OBJ_PREV
		Event {
			if_selected
			key $VK_UP

			$EV_CHANGE_OBJ
		}
	}

