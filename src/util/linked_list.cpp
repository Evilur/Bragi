#include "linked_list.h"
#include "player/guild_player.h"

template<typename T>
void LinkedList<T>::Append(T element) {
	/* If the list was empty */
	if (!_head) {
		_head = new Node(element);
		_tail = _head;
		return;
	}

	/* If the list was NOT empty, add a pointer of the new node to the last one
	 * and replace the last node with the new one */
	_tail->next = new Node(element);
	_tail = _tail->next;
}

/* Use the list only for this types */
template
class LinkedList<GuildPlayer*>;

template
class LinkedList<Track*>;