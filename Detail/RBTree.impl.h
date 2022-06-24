#ifndef _RBTREE_IMPL_H_
#define _RBTREE_IMPL_H_

namespace TinySTL {
	void __rb_tree_base_iterator::increment() {
		if (node->right != 0) {
			node = node->right;
			while (node->left != 0) node = node->left;
		}
		else {
			base_ptr y = node->parent;
			while (node == y->right) {
				node = y;
				y = y->parent;
			}
			if (node->right != y) node = y;
		}
	}

	void __rb_tree_base_iterator::decrement() {
		if (node->color == __rb_tree_red && node->parent->parent == node)
			node = node->right;
		else if (node->left != 0) {
			base_ptr y = node->left;
			while (y->right != 0) y = y->right;
			node = y;
		}
		else {
			base_ptr y = node->parent;
			while (node == y->left) {
				node = y;
				y = y->parent;
			}
			node = y;
		}
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
		__rb_tree_node_base* y = x->right;
		x->right = y->left;
		if (y->left != 0) y->left->parent = x;
		y->parent = x->parent;

		if (x == root)
			root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		y->left = x;
		x->parent = y;

	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
		__rb_tree_node_base* y = x->left;
		x->left = y->right;
		if (y->right != 0) y->right->parent = x;
		y->parent = x->parent;

		if (x == root)
			root = y;
		else if (x == x->parent->right)
			x->parent->right = y;
		else
			x->parent->left = y;
		y->right = x;
		x->parent = y;
	}


	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
		x->color = __rb_tree_red;
		while (x != root && x->parent->color == __rb_tree_red) {
			if (x->parent == x->parent->parent->left) {
				__rb_tree_node_base* y = x->parent->parent->right;
				if (y && y->color == __rb_tree_red) {
					x->parent->color = __rb_tree_black;
					y->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					x = x->parent->parent;
				}
				else {
					if (x == x->parent->right) {
						x = x->parent;
						__rb_tree_rotate_left(x, root);
					}
					x->parent->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_right(x->parent->parent, root);
				}
			}
			else {
				__rb_tree_node_base* y = x->parent->parent->left;
				if (y && y->color == __rb_tree_red) {
					x->parent->color = __rb_tree_black;
					y->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					x = x->parent->parent;
				}
				else {
					if (x == x->parent->left) {
						x = x->parent;
						__rb_tree_rotate_right(x, root);
					}
					x->parent->color = __rb_tree_black;
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_left(x->parent->parent, root);
				}
			}
		}
		root->color = __rb_tree_black;
	}


	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__insert(base_ptr x_, base_ptr y_, const value_type& v) {
		link_type x = (link_type)x_;
		link_type y = (link_type)y_;
		link_type z;

		if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y))) {
			z = create_node(v);
			left(y) = z;
			if (y == header) {
				root() = z;
				rightmost() = z;
			}
			else if (y == leftmost()) leftmost() = z;
		}
		else {
			z = create_node(v);
			right(y) = z;
			if (y == rightmost()) rightmost() = z;
		}
		parent(z) = y;
		left(z) = 0;
		right(z) = 0;

		__rb_tree_rebalance(z, header->parent);
		++node_count;
		return iterator(z);
	}


	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::link_type
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__copy(link_type x, link_type p) {
		link_type top = clone_node(x);
		top->parent = p;

		if (x->right) top->right = __copy(right(x), top);
		p = top;
		x = left(x);

		while (x != 0) {
			link_type y = clone_node(x);
			p->left = y;
			y->parent = p;
			if (x->right) y->right = __copy(right(x), y);
			p = y;
			x = left(x);
		}
		return top;
	}


	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__erase(link_type x) {
		while (x != 0) {
			__erase(right(x));
			link_type y = left(x);
			destroy_node(x);
			x = y;
		}
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline __rb_tree_node_base* rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__rb_tree_rebalance_for_erase(__rb_tree_node_base* x, __rb_tree_node_base*& root, __rb_tree_node_base*& _leftmost, __rb_tree_node_base*& _rightmost) {
		__rb_tree_node_base* y = x;
		__rb_tree_node_base* _x = 0;
		__rb_tree_node_base* _x_parent = 0;
		if (y->left == 0)
			_x = y->right;
		else if (y->right == 0)
			_x = y->left;
		else {
			y = y->right;
			while (y->left != 0) y = y->left;
			_x = y->right;
		}
		if (y != x) {
			x->left->parent = y;
			y->left = x->left;
			if (y != x->right) {
				_x_parent = y->parent;
				if (_x) _x->parent = y->parent;
				y->parent->left = _x;
				y->right = x->right;
				x->right->parent = y;
			}
			else
				_x_parent = y;
			if (root == x)
				root = y;
			else if (x->parent->left == x)
				x->parent->left = y;
			else
				x->parent->right = y;
			y->parent = x->parent;
			TinySTL::swap(y->color, x->color);
			y = x;
		}
		else {
			_x_parent = y->parent;
			if (_x) _x->parent = y->parent;
			if (root == x)
				root = _x;
			else if (x->parent->left == x)
				x->parent->left = _x;
			else
				x->parent->right = _x;
			if (_leftmost == x)
				if (x->right == 0)
					_leftmost = x->parent;

				else
					_leftmost = __rb_tree_node_base::minimum(_x);
			if (_rightmost == x)
				if (x->left == 0)
					_rightmost = x->parent;

				else
					_rightmost = __rb_tree_node_base::maximum(_x);
		}
		if (y->color != __rb_tree_red) {
			while (_x != root && (_x == 0 || _x->color == __rb_tree_black))
				if (_x == _x_parent->left) {
					__rb_tree_node_base* w = _x_parent->right;
					if (w->color == __rb_tree_red) {
						w->color = __rb_tree_black;
						_x_parent->color = __rb_tree_red;
						__rb_tree_rotate_left(_x_parent, root);
						w = _x_parent->right;
					}
					if ((w->left == 0 || w->left->color == __rb_tree_black) &&
						(w->right == 0 || w->right->color == __rb_tree_black)) {
						w->color = __rb_tree_red;
						_x = _x_parent;
						_x_parent = _x_parent->parent;
					}
					else {
						if (w->right == 0 || w->right->color == __rb_tree_black) {
							if (w->left) w->left->color = __rb_tree_black;
							w->color = __rb_tree_red;
							__rb_tree_rotate_right(w, root);
							w = _x_parent->right;
						}
						w->color = _x_parent->color;
						_x_parent->color = __rb_tree_black;
						if (w->right) w->right->color = __rb_tree_black;
						__rb_tree_rotate_left(_x_parent, root);
						break;
					}
				}
				else {
					__rb_tree_node_base* w = _x_parent->left;
					if (w->color == __rb_tree_red) {
						w->color = __rb_tree_black;
						_x_parent->color = __rb_tree_red;
						__rb_tree_rotate_right(_x_parent, root);
						w = _x_parent->left;
					}
					if ((w->right == 0 || w->right->color == __rb_tree_black) &&
						(w->left == 0 || w->left->color == __rb_tree_black)) {
						w->color = __rb_tree_red;
						_x = _x_parent;
						_x_parent = _x_parent->parent;
					}
					else {
						if (w->left == 0 || w->left->color == __rb_tree_black) {
							if (w->right) w->right->color = __rb_tree_black;
							w->color = __rb_tree_red;
							__rb_tree_rotate_left(w, root);
							w = _x_parent->left;
						}
						w->color = _x_parent->color;
						_x_parent->color = __rb_tree_black;
						if (w->left) w->left->color = __rb_tree_black;
						__rb_tree_rotate_right(_x_parent, root);
						break;
					}
				}
			if (_x) _x->color = __rb_tree_black;
		}
		return y;
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator position) {
		link_type y = (link_type)__rb_tree_rebalance_for_erase(
			position.node, header->parent, header->left, header->right);
		destroy_node(y);
		--node_count;
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::size_type rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(const key_type& x) {
		pair<iterator, iterator> p = equal_range(x);
		size_type n = distance(p.first, p.second);
		erase(p.first, p.second);
		return n;
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {
		if (this != &x) {
			clear();
			node_count = 0;
			key_compare = x.key_compare;
			if (x.root() == 0) {
				root() = 0;
				leftmost() = header;
				rightmost() = header;
			}
			else {
				root() = __copy(x.root(), header);
				leftmost() = minimum(root());
				rightmost() = maximum(root());
				node_count = x.node_count;
			}
		}
		return *this;
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, bool>
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const value_type& v) {
		link_type y = header;
		link_type x = root();
		bool comp = true;
		while (x != 0) {
			y = x;
			comp = key_compare(KeyOfValue()(v), key(x));
			x = comp ? left(x) : right(x);
		}

		iterator j = iterator(y);
		if (comp)
			if (j == begin())
				return pair<iterator, bool>(__insert(x, y, v), true);
			else
				--j;
		if(key_compare(key(j.node),KeyOfValue()(v)))
			return pair<iterator, bool>(__insert(x, y, v), true);
		return pair<iterator, bool>(j, false);
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const value_type& v) {
		link_type y = header;
		link_type x = root();
		while (x != 0) {
			y = x;
			x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
		}
		return __insert(x, y, v);
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(iterator position, const value_type& x) {
		if (position.node == header->left) {
			if (size() > 0 && key_compare(KeyOfValue()(x), key(position.node)))
				return __insert(position.node, position.node, x);
			else
				return insert_unique(x).first;
		}
		else if (position.node == header) {
			if (key_compare(key(rightmost()), KeyOfValue()(x)))
				return __insert(0, rightmost(), x);
			else
				return insert_unique(x).first;
		}
		else {
			iterator before = position;
			--before;
			if (key_compare(key(before.node), KeyOfValue()(x)) &&
				key_compare(KeyOfValue()(x), key(position.node))) {
				if (right(before.node) == 0)
					return __insert(0, before.node, x);
				else
					return __insert(position.node, position.node, x);
			}
			else
				return insert_unique(x).first;
		}
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(iterator position, const value_type& x) {
		if (position.node == header->left) {
			if (size() > 0 && !key_compare(key(position.node), KeyOfValue()(x)))
				return __insert(position.node, position.node, x);
			else
				return insert_equal(x);
		}
		else if (position.node == header) {
			if (!key_compare(KeyOfValue()(x), key(rightmost())))
				return __insert(0, rightmost(), x);
			else
				return insert_equal(x);
		}
		else {
			iterator before = position;
			--before;
			if (!key_compare(KeyOfValue()(x), key(before.node)) &&
				!key_compare(key(position.node), KeyOfValue()(x))) {
				if (right(before.node) == 0)
					return __insert(0, before.node, x);
				else
					return __insert(position.node, position.node, x);
			}
			else
				return insert_equal(x);
		}
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	template<class InputIterator>
	inline void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(InputIterator first, InputIterator last) {
		for (; first != last; ++first) insert_unique(*first);
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	template<class InputIterator>
	inline void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(InputIterator first, InputIterator last) {
		for (; first != last; ++first) insert_equal(*first);
	}


	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& k) {
		link_type y = header;
		link_type x = root();

		while (x != 0) {
			if (!key_compare(key(x), k))
				y = x, x = left(x);
			else
				x = right(x);
		}

		iterator j = iterator(y);
		if (j == end() || key_compare(k, key(j.node))) return end();
		return j;
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator, typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator> rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::equal_range(const key_type& x) {
		return pair<iterator, iterator>(lower_bound(x), upper_bound(x));
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator, typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator> rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::equal_range(const key_type& x) const {
		return pair<const_iterator, const_iterator>(lower_bound(x), upper_bound(x));
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const key_type& k) {
		link_type y = header;
		link_type x = root();

		while (x != 0)
			if (!key_compare(key(x), k))
				y = x, x = left(x);
			else
				x = right(x);

		return iterator(y);
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::lower_bound(const key_type& k) const {
		link_type y = header;
		link_type x = root();

		while (x != 0)
			if (!key_compare(key(x), k))
				y = x, x = left(x);
			else
				x = right(x);

		return const_iterator(y);
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const key_type& k) {
		link_type y = header;
		link_type x = root();

		while (x != 0)
			if (key_compare(k, key(x)))
				y = x, x = left(x);
			else
				x = right(x);

		return iterator(y);
	}

	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	inline typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::const_iterator rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::upper_bound(const key_type& k) const {
		link_type y = header;
		link_type x = root();

		while (x != 0)
			if (key_compare(k, key(x)))
				y = x, x = left(x);
			else
				x = right(x);

		return const_iterator(y);
	}

}

#endif
