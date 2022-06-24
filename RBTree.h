#ifndef _RBTREE_H_
#define _RBTREE_H_

#include "Allocator.h"
#include "Iterator.h"
#include "Utility.h"
#include "Algorithm.h"

namespace TinySTL {
	typedef bool __rb_tree_color_type;
	const __rb_tree_color_type __rb_tree_red = false;
	const __rb_tree_color_type __rb_tree_black = true;

	struct __rb_tree_node_base {
		typedef __rb_tree_color_type color_type;
		typedef __rb_tree_node_base* base_ptr;

		color_type color;
		base_ptr parent;
		base_ptr left;
		base_ptr right;

		static base_ptr minimum(base_ptr x) {
			while (x->left != 0) x = x->left;
			return x;
		}

		static base_ptr maximum(base_ptr x) {
			while (x->right != 0) x = x->right;
			return x;
		}
	};

	template <class Value>
	struct __rb_tree_node :public __rb_tree_node_base {
		typedef __rb_tree_node<Value>* link_type;
		Value value_field;
	};

	struct __rb_tree_base_iterator {
		typedef __rb_tree_node_base::base_ptr base_ptr;
		typedef bidirectional_iterator_tag iterator_category;
		typedef ptrdiff_t difference_type;

		base_ptr node;

		void increment();
		void decrement();
	};

	inline bool operator==(const __rb_tree_base_iterator& _x, const __rb_tree_base_iterator& _y) {
		return _x.node == _y.node;
	}
	inline bool operator!=(const __rb_tree_base_iterator& _x, const __rb_tree_base_iterator& _y) {
		return _x.node != _y.node;
	}

	template<class Value, class Ref, class Ptr>
	struct __rb_tree_iterator :public __rb_tree_base_iterator {
		typedef Value value_type;
		typedef Ref reference;
		typedef Ptr pointer;
		typedef __rb_tree_iterator<Value, Value&, Value*> iterator;
		typedef __rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
		typedef __rb_tree_iterator<Value, Ref, Ptr> self;
		typedef __rb_tree_node<Value>* link_type;

		__rb_tree_iterator(){}
		__rb_tree_iterator(link_type x) { node = x; }
		__rb_tree_iterator(const iterator& it) { node = it.node; }

		reference operator*()const { return link_type(node)->value_field; }
		pointer operator->()const { return &(operator*()); }

		self& operator++() { increment(); return *this; }
		self operator++(int) {
			self tmp = *this;
			increment();
			return tmp;
		}

		self& operator--() { decrement(); return *this; }
		self operator--(int) {
			self tmp = *this;
			decrement();
			return tmp;
		}
	};

	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc = alloc>
	class rb_tree {
	protected:
		typedef void* void_pointer;
		typedef __rb_tree_node_base* base_ptr;
		typedef __rb_tree_node<Value> rb_tree_node;
		typedef simple_alloc<rb_tree_node, Alloc> rb_tree_node_allocator;
		typedef __rb_tree_color_type color_type;

	public:
		typedef Key key_type;
		typedef Value value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef rb_tree_node* link_type;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

	protected:
		link_type get_node() {
			return rb_tree_node_allocator::allocate();
		}
		void put_node(link_type p) {
			rb_tree_node_allocator::deallocate(p);
		}

		link_type create_node(const value_type& x) {
			link_type tmp = get_node();
			construct(&tmp->value_field, x);
			return tmp;
		}

		link_type clone_node(link_type x) {
			link_type tmp = create_node(x->value_field);
			tmp->color = x->color;
			tmp->left = 0;
			tmp->right = 0;
			return tmp;
		}

		void destroy_node(link_type p) {
			destroy(&p->value_field);
			put_node(p);
		}

	protected:
		size_type node_count;
		link_type header;
		Compare key_compare;

		link_type& root()const { return (link_type&)header->parent; }
		link_type& leftmost()const { return (link_type&)header->left; }
		link_type& rightmost()const { return (link_type&)header->right; }

		static link_type& left(link_type x) { return (link_type&)(x->left); }
		static link_type& right(link_type x) { return (link_type&)(x->right); }
		static link_type& parent(link_type x) { return (link_type&)(x->parent); }
		static reference value(link_type x) { return x->value_field; }
		static const Key& key(link_type x) { return KeyOfValue()(value(x)); }
		static color_type& color(link_type x) { return (color_type&)(x->color); }

		static link_type& left(base_ptr x) { return (link_type&)(x->left); }
		static link_type& right(base_ptr x) { return (link_type&)(x->right); }
		static link_type& parent(base_ptr x) { return (link_type&)(x->parent); }
		static reference value(base_ptr x) { return ((link_type)x)->value_field; }
		static const Key& key(base_ptr x) { return KeyOfValue()(value(link_type(x))); }
		static color_type& color(base_ptr x) { return (color_type&)(((link_type)x)->color); }

		static link_type minimum(link_type x) {
			return (link_type)__rb_tree_node_base::minimum(x);
		}
		static link_type maximum(link_type x) {
			return (link_type)__rb_tree_node_base::maximum(x);
		}

	public:
		typedef __rb_tree_iterator<Value, Value&, Value*> iterator;
		typedef __rb_tree_iterator<const Value, const Value&, const Value*> const_iterator;

	private:
		void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root);
		void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root);
		void __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root);
		iterator __insert(base_ptr x, base_ptr y, const value_type& v);
		link_type __copy(link_type x, link_type p);
		void __erase(link_type x);
		__rb_tree_node_base* __rb_tree_rebalance_for_erase(__rb_tree_node_base* x, __rb_tree_node_base*& root, __rb_tree_node_base*& _leftmost, __rb_tree_node_base*& _rightmost);
		void init() {
			header = get_node();
			color(header) = __rb_tree_red;
			root() = 0;
			leftmost() = header;
			rightmost() = header;
		}

	public:
		rb_tree(const Compare& comp = Compare()) :node_count(0), key_compare(comp) { init(); }

		~rb_tree() {
			clear();
			put_node(header);
		}

		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
		operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);

	public:
		Compare key_comp()const { return key_compare; }
		iterator begin() { return leftmost(); }
		const_iterator begin() const { return leftmost(); }
		iterator end() { return header; }
		const_iterator end() const { return header; }
		bool empty() const { return node_count == 0; }
		size_type size() const { return node_count; }
		size_type max_size() const { return size_type(-1); }
		pair<iterator, iterator> equal_range(const key_type& x);
		pair<const_iterator, const_iterator> equal_range(const key_type& x) const;
		iterator lower_bound(const key_type& x);
		const_iterator lower_bound(const key_type& x) const;
		iterator upper_bound(const key_type& x);
		const_iterator upper_bound(const key_type& x) const;

	public:
		void erase(iterator position);
		size_type erase(const key_type& x);
		void erase(iterator first, iterator last) {
			if (first == begin() && last == end()) clear();
			else
				while (first != last) erase(first++);
		}
		pair<iterator, bool> insert_unique(const value_type& x);
		iterator insert_equal(const value_type& x);
		iterator find(const Key& k);
		void clear() {
			if (node_count != 0) {
				__erase(root());
				leftmost() = header;
				root() = 0;
				rightmost() = header;
				node_count = 0;
			}
		}
	};

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline bool operator==(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
		return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline bool operator!=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x, const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& y) {
		return !(x == y);
	}


}

#include "Detail\RBTree.impl.h"

#endif
