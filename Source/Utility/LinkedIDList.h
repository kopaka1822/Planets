#pragma once
#include <vector>
#include <assert.h>

template <class T>
class LinkedIDList
{
class Link
	{
	public:

	private:
		friend LinkedIDList;
		friend class element;
		Link(Link* previous, T* element, unsigned int id, LinkedIDList* parent)
			:
			ID(id)
		{
			prev = previous;
			data = element;
			owner = parent;
			owner->last = this;
			owner->len++;
			next = nullptr;
		}
		Link(Link* previous, Link* nxt, T* element, unsigned int id, LinkedIDList* parent)
			:
			ID(id)
		{
			prev = previous;
			next = nxt;
			owner = parent;
			owner->len++;
			data = element;
		}
		~Link()
		{
			delete data;
			data = nullptr;

			if (next != nullptr)
				next->prev = this->prev;
			else
				owner->last = this->prev;

			if (prev != nullptr)
				prev->next = this->next;
			else
				owner->first = next;

			owner->len--;
		}

		void Add(T* element, unsigned int newID)
		{
			next = new Link(this, element, newID,owner);
		}

	private:
		Link* prev;
		Link* next;
		T* data;
		const unsigned int ID;
		LinkedIDList* owner;
	};
public:
	class element
	{
		friend LinkedIDList;
		element(Link* first)
		{
			current = first;
		}

		Link* current;
	public:
		element& operator++()
		{
			assert(this);
			if (current)
				current = current->next;

			return *this;
		}
		element& operator--()
		{
			current = current->prev;

			return *this;
		}
		T* operator->()
		{
			return (current->data);
		}
		T& operator*()
		{
			return *(current->data);
		}
		bool operator==(const element& rhs) const
		{
			return (current == rhs.current);
		}
		bool operator!=(const element& rhs) const
		{
			return !(*this == rhs);
		}
		element& operator=(const element& rhs)
		{
			this->current = rhs.current;
			return *this;
		}
		inline unsigned int GetID() const
		{
			return current->ID;
		}
		void remove() //set element to previous element
		{
			Link* tmp = nullptr;
			if (current->prev != nullptr)
			{
				tmp = current->prev;
			}
			else
			{
				tmp = current->next;
			}
			delete current;
			current = tmp;
		}
	};
public:
	LinkedIDList()
	{}

	void add(T* element)
	{
		if (first == nullptr)
		{
			first = new Link(nullptr, element, curID, this);
		}
		else
		{
			last->Add(element,curID);
		}
		curID++;
	}
	LinkedIDList(const LinkedIDList& copy)
	{
		//only supports copying from an emtpy container because im lazy
		assert(copy.first == nullptr);
		first = copy.first;
		last = copy.last;
		len = copy.len;
		curID = copy.curID;
	}

	~LinkedIDList()
	{
		dispose();
	}


	T* operator[](unsigned int index)
	{
		element e = search(index);
		if (e.current == nullptr)
			return nullptr;
		return e.current->data;
	}
	void operator+=(T* obj)
	{
		add(obj);
	}

	element begin()
	{
		return element(first);
	}
	element end()
	{
		return element(nullptr);
	}
	element search(unsigned int id)
	{
		if (first != nullptr)
		{
			Link* cur = first;
			do
			{
				if (cur->ID == id)
					return element(cur);

				cur = cur->next;
			} while (cur);

			return element(nullptr);
		}
		return element(nullptr);
	}
	element back()
	{
		return element(last);
	}
	void remove(unsigned int id)
	{
		delete search(id).current;
	}
	bool insert(unsigned int id, T* obj)//optimized for insering at the end of the list
	{
		if (first == nullptr)
		{
			first = new Link(nullptr, obj, id, this);
			return true;
		}
		else
		{
			Link* cur = last;
			while (cur->ID > id)
			{
				cur = cur->prev;

				if (cur == nullptr)
				{
					Link* temp = new Link(nullptr, first, obj, id, this);
					first->prev = temp;
					first = temp;
					return true;
				}
			}
			Link* temp = new Link(cur, cur->next, obj, id, this);
			if (cur->next)
				cur->next->prev = temp;
			cur->next = temp;
			return true;
		}
	}
	unsigned int length() const
	{
		return len;
	}
	unsigned int lastID() const
	{
		return curID - 1;
	}

	inline std::vector< T > getVector() const
	{
		std::vector< T > list;
		for (auto e = begin(), ed = end(); e != ed; e++)
		{
			list.push_back(*e);
		}
		return list;
	}
private:
	void dispose()
	{
		while (first != nullptr)
		{
			delete first;
		}
		last = nullptr;
		len = 0;
	}



	friend Link;





	friend element;

	Link* first = nullptr;
	Link* last = nullptr;
	unsigned int len = 0;
	unsigned int curID = 0;
};
