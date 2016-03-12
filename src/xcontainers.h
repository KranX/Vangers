/******************************************************************
				XTList
******************************************************************/

template <class Type> class XTList {
	int num_elements;
	Type* first_element;
public:

		XTList();
		~XTList();

	int size();
	Type* first();
	Type* last();

	void clear();
	void delete_all();

	void insert(Type* p);
	void append(Type* p);
	void insert(Type* pointer,Type* p);
	void append(Type* pointer,Type* p);
	void remove(Type* p);
	Type* search(int ID);
};

template <class Type>
inline XTList<Type>::XTList()
{
	num_elements = 0; first_element = 0;
}

template <class Type>
inline XTList<Type>::~XTList()
{
	clear();
}

template <class Type>
inline void XTList<Type>::clear()
{
	while(first())
		remove(first());
}

template <class Type>
inline void XTList<Type>::delete_all()
{
	Type* p;
	while((p = first()) != 0){
		remove(p);
		delete p;
		}
}

template <class Type>
inline int XTList<Type>::size()
{
	return num_elements;
}

template <class Type>
inline Type* XTList<Type>::first()
{
	return first_element;
}

template <class Type>
inline Type* XTList<Type>::last()
{
	return first_element -> prev;
}

template <class Type>
inline void XTList<Type>::insert(Type* p)
{
	if(p -> list)
		ErrH.Abort("Element is already in list");
	num_elements++;
	if(first_element){
		p -> next = first_element;
		p -> prev = first_element -> prev;
		first_element -> prev = p;
		}
	else{
		p -> prev = p;
		p -> next = 0;
		}
	first_element = p;
	p -> list = this;
}

template <class Type>
inline void XTList<Type>::insert(Type* pointer,Type* p)
{
	if(!first_element || first_element == pointer){
		insert(p);
		return;
		}
	if(!pointer){
		append(p);
		return;
		}

	if(p -> list)
		ErrH.Abort("Element is already in list");
	num_elements++;
	p -> next = pointer;
	p -> prev = pointer -> prev;
	pointer -> prev -> next = p;
	pointer -> prev = p;
	p -> list = this;
}


template <class Type>
inline void XTList<Type>::append(Type* p)
{
	if(p -> list)
		ErrH.Abort("Element is already in list");
	num_elements++;
	if(first_element){
		p -> next = 0;
		p -> prev = first_element -> prev;
		first_element -> prev -> next = p;
		first_element -> prev = p;
		}
	else{
		p -> next = 0;
		p -> prev = first_element = p;
		}
	p -> list = this;
}

template <class Type>
inline void XTList<Type>::remove(Type* p)
{
	if(p -> list != this)
		ErrH.Abort("Removed element isn't in list");
	num_elements--;
	if(p -> next)
		p -> next -> prev = p -> prev;
	else
		first_element -> prev = p -> prev;

	if(p != first_element)
		p -> prev -> next = p -> next;
	else{
		first_element = p -> next;
		if(first_element)
			first_element -> prev = p -> prev;
		}

	p -> next = p -> prev = 0;
	p -> list = 0;
}

template <class Type>
inline Type* XTList<Type>::search(int ID)
{
	Type* p = first();
	while(p){
		if(p -> ID == ID)
			return p;
		p = p -> next;
		}
	return 0;
}

/******************************************************************
				XTListAlt
******************************************************************/
template <class Type> class XTListAlt {
	int num_elements;
	Type* first_element;
public:

		XTListAlt();
		~XTListAlt();

	int size();
	Type* first();
	Type* last();

	void clear();
	void delete_all();

	void insert(Type* p);
	void insert(Type* pointer,Type* p);
	void append(Type* p);
	void remove(Type* p);
	Type* search(int ID);
};


template <class Type>
inline XTListAlt<Type>::XTListAlt()
{
	num_elements = 0; first_element = 0;
}

template <class Type>
inline XTListAlt<Type>::~XTListAlt()
{
	clear();
}

template <class Type>
inline void XTListAlt<Type>::clear()
{
	while(first())
		remove(first());
}

template <class Type>
inline void XTListAlt<Type>::delete_all()
{
	Type* p;
	while((p = first()) != 0){
		remove(p);
		delete p;
		}
}


template <class Type>
inline int XTListAlt<Type>::size()
{
	return num_elements;
}

template <class Type>
inline Type* XTListAlt<Type>::first()
{
	return first_element;
}

template <class Type>
inline Type* XTListAlt<Type>::last()
{
	return first_element -> prev_alt;
}

template <class Type>
inline void XTListAlt<Type>::insert(Type* p)
{
	if(p -> list_alt)
		ErrH.Abort("Element is already in list");
	num_elements++;
	if(first_element){
		p -> next_alt = first_element;
		p -> prev_alt = first_element -> prev_alt;
		first_element -> prev_alt = p;
		}
	else{
		p -> prev_alt = p;
		p -> next_alt = 0;
		}
	first_element = p;
	p -> list_alt = this;
}

template <class Type>
inline void XTListAlt<Type>::insert(Type* pointer,Type* p)
{
	if(!first_element || first_element == pointer){
		insert(p);
		return;
		}
	if(!pointer){
		append(p);
		return;
		}
	if(p -> list)
		ErrH.Abort("Element is already in list");
	num_elements++;
	p -> next_alt = pointer;
	p -> prev_alt = pointer -> prev_alt;
	pointer -> prev_alt -> next_alt = p;
	pointer -> prev_alt = p;
	p -> list_alt = this;
}


template <class Type>
inline void XTListAlt<Type>::append(Type* p)
{
	if(p -> list_alt)
		ErrH.Abort("Element is already in list");
	num_elements++;
	if(first_element){
		p -> next_alt = 0;
		p -> prev_alt = first_element -> prev_alt;
		first_element -> prev_alt -> next_alt = p;
		first_element -> prev_alt = p;
		}
	else{
		p -> next_alt = 0;
		p -> prev_alt = first_element = p;
		}
	p -> list_alt = this;
}

template <class Type>
inline void XTListAlt<Type>::remove(Type* p)
{
	if(p -> list_alt != this)
		ErrH.Abort("Removed element isn't in list");
	num_elements--;
	if(p -> next_alt)
		p -> next_alt -> prev_alt = p -> prev_alt;
	else
		first_element -> prev_alt = p -> prev_alt;

	if(p != first_element)
		p -> prev_alt -> next_alt = p -> next_alt;
	else{
		first_element = p -> next_alt;
		if(first_element)
			first_element -> prev_alt = p -> prev_alt;
		}

	p -> next_alt = p -> prev_alt = 0;
	p -> list_alt = 0;
}

template <class Type>
inline Type* XTListAlt<Type>::search(int ID)
{
	Type* p = first();
	while(p){
		if(p -> ID == ID)
			return p;
		p = p -> next_alt;
		}
	return 0;
}

/******************************************************************
				XQueue
******************************************************************/
template <class Type> class XQueue {
	int size;
	int head,tail;
	Type* buffer;
public:

		XQueue(int size); // must be 2^n

	int tell();
	int empty();
//	int full();
	void clear();

	int simple_put(Type p);
	Type simple_get();

	int put(Type p);  // The second element with the same p removes the first, but remains at the end of queue
	Type get();
};

template <class Type>
inline XQueue<Type>::XQueue(int sz)
{
	if(!IsPowerOf2(sz))
		ErrH.Abort("XQueue's size must be a power of 2",sz);
	buffer = new Type[size = sz];
	clear();
}

template <class Type>
inline void XQueue<Type>::clear()
{
	head = tail = 0;
}

template <class Type>
inline int XQueue<Type>::tell()
{
	return size + head - tail & size - 1;
}

template <class Type>
inline int XQueue<Type>::empty()
{
	return head == tail;
}

/*template <class Type>
inline int XQueue<Type>::full()
{
	return (head + 1) & (size - 1) == last;
}*/

template <class Type>
inline int XQueue<Type>::put(Type p)
{
	int i = tail;
	while(i != head){
		if(buffer[i] == p){
			buffer[i] = (Type)(-1);
			break;
			}
		i = (i + 1) & (size - 1);
		}
	int new_head = (head + 1) & (size - 1);
	if(new_head != tail){
		buffer[head] = p;
		head = new_head;
		return 1;
		}
	ErrH.Abort("XQueue overflow");
	return 0;
}
template <class Type>
inline Type XQueue<Type>::get()
{
	int ind;
	while(head != tail){
		ind = tail;
		tail = (tail + 1) & (size - 1);
		if(buffer[ind] != (Type)(-1))
			return buffer[ind];
		}
	return (Type)0;
}

template <class Type>
inline int XQueue<Type>::simple_put(Type p)
{
	buffer[head] = p;
	head = (head + 1) & (size - 1);
	if(head == tail)
		tail = (tail + 1) & (size - 1);
	return 1;
}
template <class Type>
inline Type XQueue<Type>::simple_get()
{
	int ind;
	if(head != tail){
		ind = tail;
		tail = (tail + 1) & (size - 1);
		return buffer[ind];
		}
	return (Type)0;
}

/******************************************************************
				XIQueue
******************************************************************/
template <class Type> class XIQueue {
	int size;
	int head,tail;
	struct Node {
		int ID;
		Type data;
		};
	Node* buffer;
public:

		XIQueue(int size); // must be 2^n

	int empty();
//	int full();
	void clear();

	Type put(Type p,int ID); // The second element with the same ID removes the first, but remains at the end of queue.
					      // Returns previous data
	Type get();
};

template <class Type>
inline XIQueue<Type>::XIQueue(int sz)
{
	if((1 << BitSR(sz)) != sz)
		ErrH.Abort("XIQueue's size must be a power of 2",sz);
	buffer = new Node[size = sz];
	clear();
}

template <class Type>
inline void XIQueue<Type>::clear()
{
	head = tail = 0;
}

template <class Type>
inline int XIQueue<Type>::empty()
{
	return head == tail;
}

/*template <class Type>
inline int XIQueue<Type>::full()
{
	return (head + 1) & (size - 1) == last;
}*/

template <class Type>
inline Type XIQueue<Type>::put(Type p,int ID)
{
	int i = tail;
	Type prev_data = (Type)0;
	while(i != head){
		if(buffer[i].ID == ID){
			buffer[i].ID = -1;
			prev_data = buffer[i].p;
			break;
			}
		i = (i + 1) & (size - 1);
		}
	int new_head = (head + 1) & (size - 1);
	if(new_head != tail){
		buffer[head].ID = ID;
		buffer[head].data = p;
		head = new_head;
		return prev_data;
		}
	ErrH.Abort("XIQueue overflow");
	return (Type)0;
}
template <class Type>
inline Type XIQueue<Type>::get()
{
	int ind;
	while(head != tail){
		ind = tail;
		tail = (tail + 1) & (size - 1);
		if(buffer[ind].ID != -1)
			return buffer[ind].data;
		}
	return (Type)0;
}
