#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H

#include <list>
#include <optional>
#include <stack>
#include <vector>
#include <new>
#include <logger.h>
#include <logger_guardant.h>
#include <allocator.h>
#include <allocator_guardant.h>
#include <not_implemented.h>
#include <search_tree.h>
#include <string>

template<
        typename tkey,
        typename tvalue>
class binary_search_tree :
        public search_tree<tkey, tvalue> {

protected:

  struct node {

  public:

    tkey key;
    tvalue value;
    node* left_subtree;
    node* right_subtree;
  public:
    explicit node(tkey const &key, tvalue const &value);
    explicit node(tkey const &key, tvalue &&value);

  public:
    virtual ~node() noexcept = default;
  };

protected:
  inline std::string get_typename() const noexcept {
    return "binary_search_tree";
  }

public:

#pragma region iterators definition

  class prefix_iterator;

  class prefix_const_iterator;

  class prefix_reverse_iterator;

  class prefix_reverse_const_iterator;

  class infix_iterator;

  class infix_const_iterator;

  class infix_reverse_iterator;

  class infix_reverse_const_iterator;

  class postfix_iterator;

  class postfix_const_iterator;

  class postfix_reverse_iterator;

  class postfix_reverse_const_iterator;

  struct iterator_data {

    friend class prefix_iterator;

    friend class prefix_const_iterator;

    friend class prefix_reverse_iterator;

    friend class prefix_reverse_const_iterator;

    friend class infix_iterator;

    friend class infix_const_iterator;

    friend class infix_reverse_iterator;

    friend class infix_reverse_const_iterator;

    friend class postfix_iterator;

    friend class postfix_const_iterator;

    friend class postfix_reverse_iterator;

    friend class postfix_reverse_const_iterator;

  public:

    unsigned int _depth;

  private:

    tkey* _key;
    tvalue* _value;

  private:

    bool _is_state_initialized;
    bool _is_related_to_tree;

  public:

    inline tkey const &get_key() const noexcept {
      if (is_state_initialized())
        return *_key;
      throw std::logic_error("[BST] [Iterator Data] [Get Key] Iterator data is not initialized\n");
    }

    inline tvalue const &get_value() const noexcept {
      if (is_state_initialized())
        return *_value;
      return *_value;
    }

    inline bool is_state_initialized() const noexcept {
      return _is_state_initialized;
    }

  public:

    iterator_data() :
            _key(reinterpret_cast<tkey*>(::operator new(sizeof(tkey), std::nothrow))),
            _value(reinterpret_cast<tvalue*>(::operator new(sizeof(tvalue), std::nothrow))),
            _is_state_initialized(false),
            _is_related_to_tree(false) {
      if (_key == nullptr || _value == nullptr) {
        throw std::bad_alloc();
      }
    }

    iterator_data(
            unsigned int depth,
            tkey const &key,
            tvalue const &value) :
            _depth(depth),
            _key(reinterpret_cast<tkey*>(::operator new(sizeof(tkey), std::nothrow))),
            _value(reinterpret_cast<tvalue*>(::operator new(sizeof(tvalue), std::nothrow))),
            _is_state_initialized(true),
            _is_related_to_tree(false) {
      if (_key == nullptr || _value == nullptr) {
        throw std::bad_alloc();
      }

      allocator::construct(_key, key);
      allocator::construct(_value, value);
    }

    explicit iterator_data(
            unsigned int depth,
            node** src_node
    ) :
            _depth(depth),
            _key(nullptr),
            _value(nullptr),
            _is_state_initialized(false),
            _is_related_to_tree(true) {
      if (*src_node != nullptr) {
        _key = new tkey;
        *_key = ((*src_node)->key);

        _value = new tvalue;
        *_value = ((*src_node)->value);
        _is_state_initialized = true;

      }
    }

  public:

    iterator_data(
            iterator_data const &other);

    iterator_data(
            iterator_data &&other) noexcept;

    iterator_data &operator=(
            iterator_data const &other);

    iterator_data &operator=(
            iterator_data &&other) noexcept;

    virtual ~iterator_data() noexcept {
      if (_is_state_initialized) {
        allocator::destruct(_key);
        allocator::destruct(_value);
      }

      ::operator delete(_key);
      _key = nullptr;
      ::operator delete(_value);
      _value = nullptr;

      _is_state_initialized = false;
    };

  };

  class prefix_iterator final {

    friend class binary_search_tree<tkey, tvalue>;

  private:

    explicit prefix_iterator(
            binary_search_tree<tkey, tvalue> const* holder,
            typename binary_search_tree<tkey, tvalue>::node* subtree_root);

  public:

    bool operator==(
            prefix_iterator const &other) const noexcept;

    bool operator!=(
            prefix_iterator const &other) const noexcept;

    prefix_iterator &operator++();

    prefix_iterator const operator++(
            int not_used);

    iterator_data* operator*() const;

  public:
    // rule of five

    prefix_iterator(
            prefix_iterator const &other);

    prefix_iterator(
            prefix_iterator &&other) noexcept;

    prefix_iterator &operator=(
            prefix_iterator const &other) = default;

    prefix_iterator &operator=(
            prefix_iterator &&other) noexcept = default;

    ~prefix_iterator() noexcept = default;

  protected:
    std::stack<node*> _path;
    binary_search_tree<tkey, tvalue> const* _holder;
    iterator_data* _data;

  public:

    void pass_to_next_node();

    void set_value(
            tvalue const &value) {
      this->_state.top().value = value;
    }

    void set_value(
            tvalue &&value) noexcept {
      this->_state.top().value = std::move(value);
    }

  };

  class prefix_const_iterator final {

    friend class binary_search_tree<tkey, tvalue>;

  private:

    explicit prefix_const_iterator(
            binary_search_tree<tkey, tvalue> const* holder,
            typename binary_search_tree<tkey, tvalue>::node* subtree_root);

  public:

    bool operator==(
            prefix_const_iterator const &other) const noexcept;

    bool operator!=(
            prefix_const_iterator const &other) const noexcept;

    prefix_const_iterator &operator++();

    prefix_const_iterator const operator++(
            int not_used);

    iterator_data* operator*() const;

  public:
    prefix_const_iterator(
            prefix_const_iterator const &other);

    prefix_const_iterator(
            prefix_const_iterator &&other) noexcept;

    prefix_const_iterator &operator=(
            prefix_const_iterator const &other) = default;

    prefix_const_iterator &operator=(
            prefix_const_iterator &&other) noexcept = default;

    ~prefix_const_iterator() noexcept = default;

  protected:
    std::stack<node*> _path;
    binary_search_tree<tkey, tvalue> const* _holder;
    iterator_data* _data;

  public:

    void assign_data() {
      _data->_key = &(_path.top()->key);
      _data->_value = &(_path.top()->value);
      _data->_is_state_initialized = true;
      _data->_depth = _path.size() - 1;

      _holder->inject_additional_data(_data, _path.top());
    }

    void pass_to_next_node();
  };

  class prefix_reverse_iterator final {

    friend class binary_search_tree<tkey, tvalue>;

  private:

    explicit prefix_reverse_iterator(
            binary_search_tree<tkey, tvalue> const* holder,
            typename binary_search_tree<tkey, tvalue>::node* subtree_root);

  private:

    std::stack<node*> _path;
    binary_search_tree<tkey, tvalue> const* _holder;
    iterator_data* _data;

  public:

    bool operator==(
            prefix_reverse_iterator const &other) const noexcept;

    bool operator!=(
            prefix_reverse_iterator const &other) const noexcept;

    prefix_reverse_iterator &operator++();

    prefix_reverse_iterator const operator++(
            int not_used);

    iterator_data* operator*() const;

  public:

    prefix_reverse_iterator(
            prefix_reverse_iterator const &other) :
            _holder(other._holder),
            _path(other._state) {
      if (_path.empty()) {
        _data = _holder->create_iterator_data();
      } else {
        _data = _holder->create_iterator_data(_path.size() - 1, _path.top());
      }
    }

    prefix_reverse_iterator(
            prefix_reverse_iterator &&other) noexcept:
            _holder(std::move(other._holder)),
            _path(std::move(other._path)),
            _data(std::move(other._data)) {
      other._holder = nullptr;
      other._data = nullptr;
    }

    prefix_reverse_iterator &operator=(
            prefix_reverse_iterator const &other) = default;

    prefix_reverse_iterator &operator=(
            prefix_reverse_iterator &&other) noexcept = default;

    ~prefix_reverse_iterator() noexcept = default;

  private:

    void assign_data() {
      _data->_key = &(_path.top()->key);
      _data->_value = &(_path.top()->value);
      _data->_is_state_initialized = true;
      _data->_depth = _path.size() - 1;

      _holder->inject_additional_data(_data, _path.top());
    }

    void pass_to_next_node();

  public:
    void set_value(
            tvalue const &value) {
      this->_state.top().value = value;
    }

    void set_value(
            tvalue &&value) noexcept {
      this->_state.top().value = std::move(value);
    }

  };

  class prefix_const_reverse_iterator final {

    friend class binary_search_tree<tkey, tvalue>;

  private:

    explicit prefix_const_reverse_iterator(
            binary_search_tree<tkey, tvalue> const* holder,
            typename binary_search_tree<tkey, tvalue>::node* subtree_root);

  public:

    bool operator==(
            prefix_const_reverse_iterator const &other) const noexcept;

    bool operator!=(
            prefix_const_reverse_iterator const &other) const noexcept;

    prefix_const_reverse_iterator &operator++();

    prefix_const_reverse_iterator const operator++(
            int not_used);

    iterator_data* operator*() const;

  public:
    prefix_const_reverse_iterator(
            prefix_const_reverse_iterator const &other);

    prefix_const_reverse_iterator(
            prefix_const_reverse_iterator &&other) noexcept;

    prefix_const_reverse_iterator &operator=(
            prefix_const_reverse_iterator const &other) = default;

    prefix_const_reverse_iterator &operator=(
            prefix_const_reverse_iterator &&other) noexcept = default;

    ~prefix_const_reverse_iterator() noexcept = default;

  private:
    std::stack<node*> _path;
    binary_search_tree<tkey, tvalue> const* _holder;
    iterator_data* _data;

  private:
    void assign_data() {
      _data->_key = &(_path.top()->key);
      _data->_value = &(_path.top()->value);
      _data->_is_state_initialized = true;
      _data->_depth = _path.size() - 1;

      _holder->inject_additional_data(_data, _path.top());
    }

    void pass_to_next_node();

  };

  class infix_iterator final {

    friend class binary_search_tree<tkey, tvalue>;

  public:

    iterator_data* _data;
    std::stack<node*> _path;
    binary_search_tree<tkey, tvalue> const* _holder;

  private:

    explicit infix_iterator(
            binary_search_tree<tkey, tvalue> const* holder,
            typename binary_search_tree<tkey, tvalue>::node* subtree_root);

  public:

    infix_iterator(
            infix_iterator const &other);

    infix_iterator(
            infix_iterator &&other) noexcept;

    infix_iterator &operator=(
            infix_iterator const &other) = default;

    infix_iterator &operator=(
            infix_iterator &&other) noexcept = default;

    ~infix_iterator() noexcept = default;

  public:

    bool operator==(
            infix_iterator const &other) const noexcept;

    bool operator!=(
            infix_iterator const &other) const noexcept;

    infix_iterator &operator++();

    infix_iterator const operator++(
            int not_used);

    iterator_data* operator*() const;

  private:

    void assign_data() {
      _data->_key = &(_path.top()->key);
      _data->_value = &(_path.top()->value);
      _data->_is_state_initialized = true;
      _data->_depth = _path.size() - 1;

      _holder->inject_additional_data(_data, _path.top());
    }

    void pass_to_next_node();

  public:
    void set_value(
            tvalue const &value) {
      this->_state.top().value = value;
    }

    void set_value(
            tvalue &&value) noexcept {
      this->_state.top().value = std::move(value);
    }

  };

  class infix_const_iterator final {

    friend class binary_search_tree<tkey, tvalue>;

  private:

    explicit infix_const_iterator(
            binary_search_tree<tkey, tvalue> const* holder,
            typename binary_search_tree<tkey, tvalue>::node* subtree_root);

    explicit infix_const_iterator(
            binary_search_tree<tkey, tvalue> const* holder,
            std::stack<node*> const &state) :
            _holder(holder),
            _path(state) {
      if (_path.empty()) {
        _data = _holder->create_iterator_data();
      } else {
        _data = _holder->create_iterator_data(_path.size() - 1, _path.top());
      }
    }

  public:

    bool operator==(
            infix_const_iterator const &other) const noexcept;

    bool operator!=(
            infix_const_iterator const &other) const noexcept;

    infix_const_iterator &operator++();

    infix_const_iterator const operator++(
            int not_used);

    iterator_data* operator*() const;

  public:
    infix_const_iterator(
            infix_const_iterator const &other);

    infix_const_iterator(
            infix_const_iterator &&other) noexcept;

    infix_const_iterator &operator=(
            infix_const_iterator const &other) = default;

    infix_const_iterator &operator=(
            infix_const_iterator &&other) noexcept = default;

    ~infix_const_iterator() noexcept = default;

  private:
    std::stack<node*> _path;
    binary_search_tree<tkey, tvalue> const* _holder;
    iterator_data* _data;

  private:
    void assign_data() {
      _data->_key = &(_path.top()->key);
      _data->_value = &(_path.top()->value);
      _data->_is_state_initialized = true;
      _data->_depth = _path.size() - 1;

      _holder->inject_additional_data(_data, _path.top());
    }

    void pass_to_next_node();

  };

  class infix_reverse_iterator final {

    friend class binary_search_tree<tkey, tvalue>;

  private:

    explicit infix_reverse_iterator(
            binary_search_tree<tkey, tvalue> const* holder,
            typename binary_search_tree<tkey, tvalue>::node* subtree_root);

  public:

    bool operator==(
            infix_reverse_iterator const &other) const noexcept;

    bool operator!=(
            infix_reverse_iterator const &other) const noexcept;

    infix_reverse_iterator &operator++();

    infix_reverse_iterator const operator++(
            int not_used);

    iterator_data* operator*() const;

  public:
    infix_reverse_iterator(
            infix_reverse_iterator const &other);

    infix_reverse_iterator(
            infix_reverse_iterator &&other) noexcept;

    infix_reverse_iterator &operator=(
            infix_reverse_iterator const &other) = default;

    infix_reverse_iterator &operator=(
            infix_reverse_iterator &&other) noexcept = default;

    ~infix_reverse_iterator() noexcept = default;

  private:
    std::stack<node*> _path;
    binary_search_tree<tkey, tvalue> const* _holder;
    iterator_data* _data;

  private:
    void assign_data() {
      _data->_key = &(_path.top()->key);
      _data->_value = &(_path.top()->value);
      _data->_is_state_initialized = true;
      _data->_depth = _path.size() - 1;

      _holder->inject_additional_data(_data, _path.top());
    }

    void pass_to_next_node();

  public:
    void set_value(
            tvalue const &value) {
      this->_state.top().value = value;
    }

    void set_value(
            tvalue &&value) noexcept {
      this->_state.top().value = std::move(value);
    }

  };

  class infix_const_reverse_iterator final {

    friend class binary_search_tree<tkey, tvalue>;

  private:

    explicit infix_const_reverse_iterator(
            binary_search_tree<tkey, tvalue> const* holder,
            typename binary_search_tree<tkey, tvalue>::node* subtree_root);

  public:

    bool operator==(
            infix_const_reverse_iterator const &other) const noexcept;

    bool operator!=(
            infix_const_reverse_iterator const &other) const noexcept;

    infix_const_reverse_iterator &operator++();

    infix_const_reverse_iterator const operator++(
            int not_used);

    iterator_data* operator*() const;

  public:
    infix_const_reverse_iterator(
            infix_const_reverse_iterator const &other);

    infix_const_reverse_iterator(
            infix_const_reverse_iterator &&other) noexcept;

    infix_const_reverse_iterator &operator=(
            infix_const_reverse_iterator const &other) = default;

    infix_const_reverse_iterator &operator=(
            infix_const_reverse_iterator &&other) noexcept = default;

    ~infix_const_reverse_iterator() noexcept = default;

  private:
    std::stack<node*> _path;
    binary_search_tree<tkey, tvalue> const* _holder;
    iterator_data* _data;

  private:
    void assign_data() {
      _data->_key = &(_path.top()->key);
      _data->_value = &(_path.top()->value);
      _data->_is_state_initialized = true;
      _data->_depth = _path.size() - 1;

      _holder->inject_additional_data(_data, _path.top());
    }

    void pass_to_next_node();

  };

  class postfix_iterator final {

    friend class binary_search_tree<tkey, tvalue>;

  private:

    explicit postfix_iterator(
            binary_search_tree<tkey, tvalue> const* holder,
            typename binary_search_tree<tkey, tvalue>::node* subtree_root);

  public:

    bool operator==(
            postfix_iterator const &other) const noexcept;

    bool operator!=(
            postfix_iterator const &other) const noexcept;

    postfix_iterator &operator++();

    postfix_iterator const operator++(
            int not_used);

    iterator_data* operator*() const;

  public:
    postfix_iterator(
            postfix_iterator const &other);

    postfix_iterator(
            postfix_iterator &&other) noexcept;

    postfix_iterator &operator=(
            postfix_iterator const &other) = default;

    postfix_iterator &operator=(
            postfix_iterator &&other) noexcept = default;

    ~postfix_iterator() noexcept = default;

  private:
    std::stack<node*> _path;
    binary_search_tree<tkey, tvalue> const* _holder;
    iterator_data* _data;

  private:
    void assign_data() {
      _data->_key = &(_path.top()->key);
      _data->_value = &(_path.top()->value);
      _data->_is_state_initialized = true;
      _data->_depth = _path.size() - 1;

      _holder->inject_additional_data(_data, _path.top());
    }

    void pass_to_next_node();

  public:
    void set_value(
            tvalue const &value) {
      this->_state.top().value = value;
    }

    void set_value(
            tvalue &&value) noexcept {
      this->_state.top().value = std::move(value);
    }

  };

  class postfix_const_iterator final {

    friend class binary_search_tree<tkey, tvalue>;

  private:

    explicit postfix_const_iterator(
            binary_search_tree<tkey, tvalue> const* holder,
            typename binary_search_tree<tkey, tvalue>::node* subtree_root);

  public:

    bool operator==(
            postfix_const_iterator const &other) const noexcept;

    bool operator!=(
            postfix_const_iterator const &other) const noexcept;

    postfix_const_iterator &operator++();

    postfix_const_iterator const operator++(
            int not_used);

    iterator_data* operator*() const;

  public:
    postfix_const_iterator(
            postfix_const_iterator const &other);

    postfix_const_iterator(
            postfix_const_iterator &&other) noexcept;

    postfix_const_iterator &operator=(
            postfix_const_iterator const &other) = default;

    postfix_const_iterator &operator=(
            postfix_const_iterator &&other) noexcept = default;

    ~postfix_const_iterator() noexcept = default;

  private:
    std::stack<node*> _path;
    binary_search_tree<tkey, tvalue> const* _holder;
    iterator_data* _data;

  private:
    void assign_data() {
      _data->_key = &(_path.top()->key);
      _data->_value = &(_path.top()->value);
      _data->_is_state_initialized = true;
      _data->_depth = _path.size() - 1;

      _holder->inject_additional_data(_data, _path.top());
    }

    void pass_to_next_node();

  };

  class postfix_reverse_iterator final {

    friend class binary_search_tree<tkey, tvalue>;

  private:

    explicit postfix_reverse_iterator(
            binary_search_tree<tkey, tvalue> const* holder,
            typename binary_search_tree<tkey, tvalue>::node* subtree_root);

  public:

    bool operator==(
            postfix_reverse_iterator const &other) const noexcept;

    bool operator!=(
            postfix_reverse_iterator const &other) const noexcept;

    postfix_reverse_iterator &operator++();

    postfix_reverse_iterator const operator++(
            int not_used);

    iterator_data* operator*() const;

  public:
    postfix_reverse_iterator(
            postfix_reverse_iterator const &other);

    postfix_reverse_iterator(
            postfix_reverse_iterator &&other) noexcept;

    postfix_reverse_iterator &operator=(
            postfix_reverse_iterator const &other) = default;

    postfix_reverse_iterator &operator=(
            postfix_reverse_iterator &&other) noexcept = default;

    ~postfix_reverse_iterator() noexcept = default;

  private:
    std::stack<node*> _path;
    binary_search_tree<tkey, tvalue> const* _holder;
    iterator_data* _data;

  private:
    void assign_data() {
      _data->_key = &(_path.top()->key);
      _data->_value = &(_path.top()->value);
      _data->_is_state_initialized = true;
      _data->_depth = _path.size() - 1;

      _holder->inject_additional_data(_data, _path.top());
    }

    void pass_to_next_node();

  public:
    void set_value(
            tvalue const &value) {
      this->_state.top().value = value;
    }

    void set_value(
            tvalue &&value) noexcept {
      this->_state.top().value = std::move(value);
    }

  };

  class postfix_const_reverse_iterator final {

    friend class binary_search_tree<tkey, tvalue>;

  private:

    explicit postfix_const_reverse_iterator(
            binary_search_tree<tkey, tvalue> const* holder,
            typename binary_search_tree<tkey, tvalue>::node* subtree_root);

  public:

    bool operator==(
            postfix_const_reverse_iterator const &other) const noexcept;

    bool operator!=(
            postfix_const_reverse_iterator const &other) const noexcept;

    postfix_const_reverse_iterator &operator++();

    postfix_const_reverse_iterator const operator++(
            int not_used);

    iterator_data* operator*() const;

  public:
    postfix_const_reverse_iterator(
            postfix_const_reverse_iterator const &other);

    postfix_const_reverse_iterator(
            postfix_const_reverse_iterator &&other) noexcept;

    postfix_const_reverse_iterator &operator=(
            postfix_const_reverse_iterator const &other) = default;

    postfix_const_reverse_iterator &operator=(
            postfix_const_reverse_iterator &&other) noexcept = default;

    ~postfix_const_reverse_iterator() noexcept = default;

  private:
    std::stack<node*> _path;
    binary_search_tree<tkey, tvalue> const* _holder;
    iterator_data* _data;

  private:
    void assign_data() {
      _data->_key = &(_path.top()->key);
      _data->_value = &(_path.top()->value);
      _data->_is_state_initialized = true;
      _data->_depth = _path.size() - 1;

      _holder->inject_additional_data(_data, _path.top());
    }

    void pass_to_next_node();

  };

#pragma endregion iterators definition

public:

#pragma region target operations strategies definition

  enum class insertion_of_existent_key_attempt_strategy {
    update_value,
    throw_an_exception
  };

  enum class disposal_of_nonexistent_key_attempt_strategy {
    do_nothing,
    throw_an_exception
  };

#pragma endregion target operations strategies definition

#pragma region target operations associated exception types

  class insertion_of_existent_key_attempt_exception final :
          public std::logic_error {

  private:

    tkey _key;

  public:

    explicit insertion_of_existent_key_attempt_exception(
            tkey const &key);

  public:

    tkey const &get_key() const noexcept;

  };

  class obtaining_of_nonexistent_key_attempt_exception final :
          public std::logic_error {

  private:

    tkey _key;

  public:

    explicit obtaining_of_nonexistent_key_attempt_exception(
            tkey const &key);

  public:

    tkey const &get_key() const noexcept;

  };

  class disposal_of_nonexistent_key_attempt_exception final :
          public std::logic_error {

  private:

    tkey _key;

  public:

    explicit disposal_of_nonexistent_key_attempt_exception(
            tkey const &key);

  public:

    tkey const &get_key() const noexcept;

  };

#pragma endregion target operations associated exception types

#pragma region template methods definition

protected:

  class template_method_basics :
          public logger_guardant {

  protected:

    binary_search_tree<tkey, tvalue>* _tree;

  public:

    explicit template_method_basics(
            binary_search_tree<tkey, tvalue>* tree);

  protected:

    std::stack<node**> find_path(
            tkey const &key) const {
      std::stack<node**> result_path;

      node** path_finder = &(_tree->_root);
      auto const &comparer = _tree->_keys_comparer;

      while (true) {
        result_path.push(path_finder);

        if (*path_finder == nullptr) {
          break;
        }

        auto comparison_result = comparer(key, (*path_finder)->key);
        if (comparison_result == 0) {
          break;
        }

        path_finder = comparison_result < 0
                      ? &((*path_finder)->left_subtree)
                      : &((*path_finder)->right_subtree);
      }

      return result_path;
    }

    virtual void balance(
            std::stack<node**> &path) {

    }

  };

  class insertion_template_method :
          public template_method_basics,
          public allocator_guardant {

  private:

    binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy _insertion_strategy;

  public:

    explicit insertion_template_method(
            binary_search_tree<tkey, tvalue>* tree,
            typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy);

  public:

    void insert(
            tkey const &key,
            tvalue const &value);

    void insert(
            tkey const &key,
            tvalue &&value);

    void set_insertion_strategy(
            typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy) noexcept;

  public:

    [[nodiscard]] allocator* get_allocator() const noexcept final;
    [[nodiscard]] logger* get_logger() const noexcept final;

  };

  class obtaining_template_method :
          public template_method_basics {

  public:

    explicit obtaining_template_method(
            binary_search_tree<tkey, tvalue>* tree);

  public:

    tvalue const &obtain(
            tkey const &key);

    std::vector<typename associative_container<tkey, tvalue>::key_value_pair> obtain_between(
            tkey const &lower_bound,
            tkey const &upper_bound,
            bool lower_bound_inclusive,
            bool upper_bound_inclusive) {
      auto const &comparer = this->_tree->_keys_comparer;
      std::vector<typename associative_container<tkey, tvalue>::key_value_pair> range;
      std::stack<node*> path;

      node* path_finder = reinterpret_cast<node*>(this->_tree->_root);
      int comparison_result;

      while (true) {
        path.push(path_finder);

        if (path_finder == nullptr || (comparison_result = comparer(lower_bound, path_finder->key)) == 0) {
          break;
        }

        path_finder = comparison_result < 0
                      ? path_finder->left_subtree
                      : path_finder->right_subtree;
      }

      if (path.top() == nullptr) {
        path.pop();

        // if came from right subtree
        if (!path.empty() && comparer(lower_bound, path.top()->key) == 1) {
          do {
            path_finder = path.top();
            path.pop();
          } while (!path.empty() && path.top()->right_subtree == path_finder);
        }
      }

      if (path.empty()) {
        return range;
      }

      if (lower_bound_inclusive) {
        node* top = path.top();
        range.push_back({top->key, top->value});
      }

      auto iter = binary_search_tree<tkey, tvalue>::infix_const_iterator(this->_tree, path);
      ++iter;

      while (iter != this->_tree->cend_infix() &&
             (comparer((*iter)->get_key(), upper_bound)) < (upper_bound_inclusive ? 1 : 0)) {
        range.push_back({(*iter)->get_key(), (*iter)->get_value()});
        ++iter;
      }

      return range;
    }

  public:
    [[nodiscard]] allocator* get_allocator() const noexcept;
    [[nodiscard]] logger* get_logger() const noexcept;

  };

  class disposal_template_method :
          public template_method_basics,
          public allocator_guardant {

  private:

    binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy _disposal_strategy;

  public:

    explicit disposal_template_method(
            binary_search_tree<tkey, tvalue>* tree,
            typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy);

  public:

    virtual tvalue dispose(
            tkey const &key);

    void set_disposal_strategy(
            typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy) noexcept;

  private:

    template<
            typename T>
    inline void swap(
            T &&one,
            T &&another) {
      T temp = std::move(one);
      one = std::move(another);
      another = std::move(temp);
    }

  private:

    [[nodiscard]] allocator* get_allocator() const noexcept;
    [[nodiscard]] logger* get_logger() const noexcept;

  };

#pragma endregion template methods definition

private:

  node* _root;

  insertion_template_method* _insertion_template;

  obtaining_template_method* _obtaining_template;

  disposal_template_method* _disposal_template;

protected:

  explicit binary_search_tree(
          typename binary_search_tree<tkey, tvalue>::insertion_template_method* insertion_template,
          typename binary_search_tree<tkey, tvalue>::obtaining_template_method* obtaining_template,
          typename binary_search_tree<tkey, tvalue>::disposal_template_method* disposal_template,
          std::function<int(tkey const &, tkey const &)>,
          allocator* allocator,
          logger* logger);

public:

  explicit binary_search_tree(
          std::function<int(tkey const &, tkey const &)> comparer = std::less<tkey>(),
          allocator* allocator = nullptr,
          logger* logger = nullptr,
          typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy = binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy::throw_an_exception,
          typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy = binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy::throw_an_exception);

public:

  binary_search_tree(
          binary_search_tree<tkey, tvalue> const &other);

  binary_search_tree(
          binary_search_tree<tkey, tvalue> &&other) noexcept;

  binary_search_tree<tkey, tvalue> &operator=(
          binary_search_tree<tkey, tvalue> const &other);

  binary_search_tree<tkey, tvalue> &operator=(
          binary_search_tree<tkey, tvalue> &&other) noexcept;

  ~binary_search_tree() override;

private:

  void clear(
          node*&subtree_root) {
    if (subtree_root == nullptr) {
      return;
    }

    clear(subtree_root->left_subtree);
    clear(subtree_root->right_subtree);
    subtree_root->~node();
    this->deallocate_with_guard(subtree_root);

    subtree_root = nullptr;
  }

  node* copy(
          node const* subtree_root) {
    if (subtree_root == nullptr) {
      return nullptr;
    }

    node* subtree_root_copied = reinterpret_cast<node*>(this->allocate_with_guard(get_node_size(), 1));
    call_node_constructor(subtree_root_copied, subtree_root->key, subtree_root->value);
    subtree_root_copied->left_subtree = copy(subtree_root->left_subtree);
    subtree_root_copied->right_subtree = copy(subtree_root->right_subtree);

    return subtree_root_copied;
  }

  virtual size_t get_node_size() const noexcept {
    return sizeof(typename binary_search_tree<tkey, tvalue>::node);
  }

  virtual void call_node_constructor(
          node* raw_space,
          tkey const &key,
          tvalue const &value) {
    allocator::construct(raw_space, key, value);
  }

  virtual void call_node_constructor(
          node* raw_space,
          tkey const &key,
          tvalue &&value) {
    allocator::construct(raw_space, key, std::move(value));
  }

  virtual void inject_additional_data(
          iterator_data* destination,
          node* source) {

  }

  virtual void inject_additional_data(
          iterator_data* destination,
          node const* source) const {

  }

  virtual iterator_data* create_iterator_data() const {
    return new iterator_data;
  }

  virtual iterator_data* create_iterator_data(
          unsigned int depth,
          node*&src_node
  ) const {
    return new iterator_data(depth, &src_node);
  }

public:

  void insert(
          tkey const &key,
          tvalue const &value) final;

  void insert(
          tkey const &key,
          tvalue &&value) final;

  tvalue const &obtain(
          tkey const &key) final;

  std::vector<typename associative_container<tkey, tvalue>::key_value_pair> obtain_between(
          tkey const &lower_bound,
          tkey const &upper_bound,
          bool lower_bound_inclusive,
          bool upper_bound_inclusive) final;

  tvalue dispose(
          tkey const &key) final;

public:

  void set_insertion_strategy(
          typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy) noexcept;

  void set_removal_strategy(
          typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy) noexcept;

public:

#pragma region iterators requests definition

  prefix_iterator begin_prefix() const noexcept;

  prefix_iterator end_prefix() const noexcept;

  prefix_const_iterator cbegin_prefix() const noexcept;

  prefix_const_iterator cend_prefix() const noexcept;

  prefix_reverse_iterator rbegin_prefix() const noexcept;

  prefix_reverse_iterator rend_prefix() const noexcept;

  prefix_const_reverse_iterator crbegin_prefix() const noexcept;

  prefix_const_reverse_iterator crend_prefix() const noexcept;

  infix_iterator begin_infix() const noexcept;

  infix_iterator end_infix() const noexcept;

  infix_const_iterator cbegin_infix() const noexcept;

  infix_const_iterator cend_infix() const noexcept;

  infix_reverse_iterator rbegin_infix() const noexcept;

  infix_reverse_iterator rend_infix() const noexcept;

  infix_const_reverse_iterator crbegin_infix() const noexcept;

  infix_const_reverse_iterator crend_infix() const noexcept;

  postfix_iterator begin_postfix() const noexcept;

  postfix_iterator end_postfix() const noexcept;

  postfix_const_iterator cbegin_postfix() const noexcept;

  postfix_const_iterator cend_postfix() const noexcept;

  postfix_reverse_iterator rbegin_postfix() const noexcept;

  postfix_reverse_iterator rend_postfix() const noexcept;

  postfix_const_reverse_iterator crbegin_postfix() const noexcept;

  postfix_const_reverse_iterator crend_postfix() const noexcept;

#pragma endregion iterators requests definition

protected:

#pragma region subtree rotations definition

  void small_left_rotation(
          typename binary_search_tree<tkey, tvalue>::node*&subtree_root,
          bool validate = true) const;

  void small_right_rotation(
          typename binary_search_tree<tkey, tvalue>::node*&subtree_root,
          bool validate = true) const;

  void big_left_rotation(
          typename binary_search_tree<tkey, tvalue>::node*&subtree_root,
          bool validate = true) const;

  void big_right_rotation(
          typename binary_search_tree<tkey, tvalue>::node*&subtree_root,
          bool validate = true) const;

  void double_left_rotation(
          typename binary_search_tree<tkey, tvalue>::node*&subtree_root,
          bool at_grandparent_first,
          bool validate = true) const;

  void double_right_rotation(
          typename binary_search_tree<tkey, tvalue>::node*&subtree_root,
          bool at_grandparent_first,
          bool validate = true) const;

#pragma endregion subtree rotations definition

};

#pragma region binary_search_tree<tkey, tvalue>::node methods implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::node::node(
        tkey const &key,
        tvalue const &value):
        key(key),
        value(value),
        left_subtree(nullptr),
        right_subtree(nullptr) {

}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::node::node(
        tkey const &key,
        tvalue &&value):
        key(key),
        value(std::move(value)),
        left_subtree(nullptr),
        right_subtree(nullptr) {

}

#pragma endregion binary_search_tree<tkey, tvalue>::node methods implementation

#pragma region iterators implementation

#pragma region iterator data implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::iterator_data::iterator_data(
        iterator_data const &other):
        _depth(other._depth),
        _is_state_initialized(other._is_state_initialized),
        _is_related_to_tree(other._is_related_to_tree) {
  if (other._is_related_to_tree) {
    _key = other._key;
    _value = other._value;

    return;
  }

  try {
    _key = reinterpret_cast<tkey*>(::operator new(sizeof(tkey)));
    _value = reinterpret_cast<tvalue*>(::operator new(sizeof(tvalue)));
  }
  catch (const std::bad_alloc &) {
    ::operator delete(_key);
    ::operator delete(_value);
    throw;
  }

  allocator::construct(_key, *other._key);
  allocator::construct(_value, *other._value);
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::iterator_data::iterator_data(
        iterator_data &&other) noexcept:
        _depth(std::move(other._depth)),
        _key(std::move(other._key)),
        _value(std::move(other._value)),
        _is_state_initialized(std::move(other._is_state_initialized)),
        _is_related_to_tree(std::move(other._is_related_to_tree)) {
  other._key = nullptr;
  other._value = nullptr;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data &binary_search_tree<tkey, tvalue>::iterator_data::operator=(
        iterator_data const &other) {
  if (this != &other) {
    _depth = other._depth;
    _is_state_initialized = other._is_state_initialized;
    _is_related_to_tree = other._is_related_to_tree;

    if (other._is_related_to_tree) {
      _key = other._key;
      _value = other._value;

      return *this;
    }

    try {
      _key = reinterpret_cast<tkey*>(::operator new(sizeof(tkey)));
      _value = reinterpret_cast<tvalue*>(::operator new(sizeof(tvalue)));
    }
    catch (const std::bad_alloc &) {
      ::operator delete(_key);
      ::operator delete(_value);
      throw;
    }

    allocator::construct(_key, *other._key);
    allocator::construct(_value, *other._value);
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data &binary_search_tree<tkey, tvalue>::iterator_data::operator=(
        iterator_data &&other) noexcept {
  if (this != &other) {
    _depth = std::move(other._depth);
    _key = std::move(other._key);
    _value = std::move(other._value);
    _is_state_initialized = std::move(other._is_state_initialized);
    _is_related_to_tree = std::move(other._is_related_to_tree);

    other._key = nullptr;
    other._value = nullptr;
  }

  return *this;
}


#pragma endregion iterator data implementation

#pragma region prefix_iterator implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_iterator::prefix_iterator(
        prefix_iterator const &other) :
        _holder(other._holder),
        _path(other._path) {
  if (_path.empty()) {
    _data = _holder->create_iterator_data();
  } else {
    _data = _holder->create_iterator_data(_path.size() - 1, _path.top());
  }
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_iterator::prefix_iterator(
        prefix_iterator &&other) noexcept :
        _holder(std::move(other._holder)),
        _path(std::move(other._path)),
        _data(std::move(other._data)) {
  other._holder = nullptr;
  other._data = nullptr;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_iterator::prefix_iterator(
        binary_search_tree<tkey, tvalue> const* holder,
        typename binary_search_tree<tkey, tvalue>::node* subtree_root) : _holder(holder) {
  node* node = subtree_root;

  if (node != nullptr) {
    this->_path.push(node);
  }

  this->_data = holder->create_iterator_data(this->_path.size() - 1, node);
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_iterator::operator==(
        typename binary_search_tree<tkey, tvalue>::prefix_iterator const &other) const noexcept {
  if (_path.empty() && other._path.empty()) {
    return true;
  }
  if (_path.empty() ^ other._path.empty()) {
    return false;
  }
  return _path.top() == other._path.top();
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_iterator::operator!=(
        typename binary_search_tree<tkey, tvalue>::prefix_iterator const &other) const noexcept {
  return !(*this == other);
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::prefix_iterator::pass_to_next_node() {
  auto &_path = this->_path;

  if (_path.empty()) {
    return; // UB
  }

  if (_path.top()->left_subtree != nullptr) {
    _path.push(_path.top()->left_subtree);
  } else if (_path.top()->right_subtree != nullptr) {
    _path.push(_path.top()->right_subtree);
  } else {
    while (true) {
      node* current = _path.top();
      _path.pop();

      if (_path.empty()) {
        return;
      }

      if (_path.top()->left_subtree == current && _path.top()->right_subtree != nullptr) {
        _path.push(_path.top()->right_subtree);
        return;
      }
    }
  }
}


template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator &
binary_search_tree<tkey, tvalue>::prefix_iterator::operator++() {
  pass_to_next_node();

  if (!_path.empty()) {
    this->assign_data();
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator const
binary_search_tree<tkey, tvalue>::prefix_iterator::operator++(
        int not_used) {
  auto it = *this;
  ++*this;
  return it;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data*
binary_search_tree<tkey, tvalue>::prefix_iterator::operator*() const {
  if (_path.empty()) {
    throw std::logic_error("Cannot get data from empty iterator");
  }

  return _data;
}

#pragma endregion prefix_iterator implementation

#pragma region prefix_const_iterator implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_const_iterator::prefix_const_iterator(
        prefix_const_iterator const &other) :
        _holder(other._holder),
        _path(other._path) {
  if (_path.empty()) {
    _data = _holder->create_iterator_data();
  } else {
    _data = _holder->create_iterator_data(_path.size() - 1, _path.top());
  }
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_const_iterator::prefix_const_iterator(
        prefix_const_iterator &&other) noexcept :
        _holder(std::move(other._holder)),
        _path(std::move(other._path)),
        _data(std::move(other._data)) {
  other._holder = nullptr;
  other._data = nullptr;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_const_iterator::prefix_const_iterator(
        binary_search_tree<tkey, tvalue> const* holder,
        typename binary_search_tree<tkey, tvalue>::node* subtree_root) : _holder(holder) {
  node* node = subtree_root;

  if (node != nullptr) {
    this->_path.push(node);
  }

  this->_data = holder->create_iterator_data(this->_path.size() - 1, node);
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator==(
        typename binary_search_tree<tkey, tvalue>::prefix_const_iterator const &other) const noexcept {
  if (_path.empty() && other._path.empty()) {
    return true;
  }
  if (_path.empty() ^ other._path.empty()) {
    return false;
  }
  return _path.top() == other._path.top();
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator!=(
        typename binary_search_tree<tkey, tvalue>::prefix_const_iterator const &other) const noexcept {
  return !(*this == other);
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::prefix_const_iterator::pass_to_next_node() {
  auto &_path = this->_path;

  if (_path.empty()) {
    return; // UB
  }

  if (_path.top()->left_subtree != nullptr) {
    _path.push(_path.top()->left_subtree);
  } else if (_path.top()->right_subtree != nullptr) {
    _path.push(_path.top()->right_subtree);
  } else {
    while (true) {
      node* current = _path.top();
      _path.pop();

      if (_path.empty()) {
        return;
      }

      if (_path.top()->left_subtree == current && _path.top()->right_subtree != nullptr) {
        _path.push(_path.top()->right_subtree);
        return;
      }
    }
  }
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_iterator &
binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator++() {
  pass_to_next_node();

  if (!_path.empty()) {
    this->assign_data();
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_iterator const
binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator++(
        int not_used) {
  auto it = *this;
  ++*this;
  return it;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data*
binary_search_tree<tkey, tvalue>::prefix_const_iterator::operator*() const {
  if (_path.empty()) {
    throw std::logic_error("Cannot get data from empty iterator");
  }

  return _data;
}

#pragma endregion prefix_const_iterator implementation

#pragma region prefix_reverse_iterator implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::prefix_reverse_iterator(
        binary_search_tree<tkey, tvalue> const* holder,
        typename binary_search_tree<tkey, tvalue>::node* subtree_root) :
        _holder(holder) {
  node* node = subtree_root;

  if (node != nullptr) {
    this->_path.push(node);
  }

  this->_data = holder->create_iterator_data(this->path.size() - 1, node);
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator==(
        typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator const &other) const noexcept {
  if (_path.empty() && other._path.empty()) {
    return true;
  }
  if (_path.empty() ^ other._path.empty()) {
    return false;
  }
  return _path.top() == other._path.top();
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator!=(
        typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator const &other) const noexcept {
  return !(*this == other);
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator &
binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator++() {
  pass_to_next_node();

  if (!_path.empty()) {
    this->assign_data();
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator const
binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator++(
        int not_used) {
  auto it = *this;
  ++*this;
  return it;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data*
binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::operator*() const {
  if (_path.empty()) {
    throw std::logic_error("Cannot get data from empty iterator");
  }

  return _data;
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::prefix_reverse_iterator::pass_to_next_node() {
  auto &_state = this->_state;

  if (_state.empty()) {
    return;
  }

  if (_state.top()->right_subtree != nullptr) {
    _state.push(_state.top()->right_subtree);
  } else if (_state.top()->left_subtree != nullptr) {
    _state.push(_state.top()->left_subtree);
  } else {
    while (true) {
      node* current = _state.top();
      _state.pop();

      if (_state.empty()) {
        return;
      }

      if (_state.top()->right_subtree == current && _state.top()->left_subtree != nullptr) {
        _state.push(_state.top()->left_subtree);
        return;
      }
    }
  }
}

#pragma endregion prefix_reverse_iterator implementation

#pragma region prefix_const_reverse_iterator implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(
        prefix_const_reverse_iterator const &other) :
        _holder(other._holder),
        _path(other._path) {
  if (_path.empty()) {
    _data = _holder->create_iterator_data();
  } else {
    _data = _holder->create_iterator_data(_path.size() - 1, _path.top());
  }
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(
        prefix_const_reverse_iterator &&other) noexcept :
        _holder(std::move(other._holder)),
        _path(std::move(other._path)),
        _data(std::move(other._data)) {
  other._holder = nullptr;
  other._data = nullptr;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(
        binary_search_tree<tkey, tvalue> const* holder,
        typename binary_search_tree<tkey, tvalue>::node* subtree_root) :
        _holder(holder) {
  node* node = subtree_root;

  if (node != nullptr) {
    this->_state.push(node);
  }

  this->_data = holder->create_iterator_data(this->_state.size() - 1, node);
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator==(
        typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator const &other) const noexcept {
  if (_path.empty() && other._path.empty()) {
    return true;
  }
  if (_path.empty() ^ other._path.empty()) {
    return false;
  }
  return _path.top() == other._path.top();
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator!=(
        typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator const &other) const noexcept {
  return !(*this == other);
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator &
binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator++() {
  pass_to_next_node();

  if (!_path.empty()) {
    this->assign_data();
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator const
binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator++(
        int not_used) {
  auto it = *this;
  ++*this;
  return it;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data*
binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator::operator*() const {
  if (_path.empty()) {
    throw std::logic_error("Can not get data from empty iterator");
  }
  return _data;
}

#pragma endregion prefix_const_reverse_iterator implementation

#pragma region infix_iterator implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::infix_iterator::infix_iterator(
        infix_iterator const &other) :
        _holder(other._holder),
        _path(other._path) {
  if (_path.empty()) {
    _data = _holder->create_iterator_data();
  } else {
    _data = _holder->create_iterator_data(_path.size() - 1, _path.top());
  }
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::infix_iterator::infix_iterator(
        infix_iterator &&other) noexcept :
        _holder(std::move(other._holder)),
        _path(std::move(other._path)),
        _data(std::move(other._data)) {
  other._holder = nullptr;
  other._data = nullptr;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::infix_iterator::infix_iterator(
        binary_search_tree<tkey, tvalue> const* holder,
        typename binary_search_tree<tkey, tvalue>::node* subtree_root) : _holder(holder) {
  while (subtree_root != nullptr) {
    this->_state.push(subtree_root);
    subtree_root = subtree_root->left_subtree;
  }

  node* node = this->_state.empty() ? nullptr : this->_state.top();
  this->_data = holder->create_iterator_data(this->_state.size() - 1, node);
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_iterator::operator==(
        typename binary_search_tree<tkey, tvalue>::infix_iterator const &other) const noexcept {
  if (_path.empty() && other._path.empty()) {
    return true;
  }
  if (_path.empty() ^ other._path.empty()) {
    return false;
  }
  return _path.top() == other._path.top();
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_iterator::operator!=(
        typename binary_search_tree<tkey, tvalue>::infix_iterator const &other) const noexcept {
  return !(*this == other);
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::infix_iterator::pass_to_next_node() {
  auto &_path = this->_path;

  if (_path.empty()) {
    return; // UB
  }

  if (_path.top()->right_subtree != nullptr) {
    _path.push(_path.top()->right_subtree);
    while (_path.top()->left_subtree != nullptr) {
      _path.push(_path.top()->left_subtree);
    }
  } else {
    while (true) {
      node* current = _path.top();
      _path.pop();
      if (_path.empty()) {
        return;
      }
      if (_path.top()->left_subtree == current) {
        return;
      }
    }
  }
}


template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator &
binary_search_tree<tkey, tvalue>::infix_iterator::operator++() {
  pass_to_next_node();

  if (!_path.empty()) {
    this->assign_data();
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator const
binary_search_tree<tkey, tvalue>::infix_iterator::operator++(
        int not_used) {
  auto it = *this;
  ++*this;
  return it;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data*
binary_search_tree<tkey, tvalue>::infix_iterator::operator*() const {
  if (_path.empty()) {
    throw std::logic_error("Cannot get data from empty iterator");
  }

  return _data;
}

#pragma endregion infix_iterator implementation

#pragma region infix_const_iterator implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::infix_const_iterator::infix_const_iterator(
        infix_const_iterator const &other) :
        _holder(other._holder),
        _path(other._path) {
  if (_path.empty()) {
    _data = _holder->create_iterator_data();
  } else {
    _data = _holder->create_iterator_data(_path.size() - 1, _path.top());
  }
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::infix_const_iterator::infix_const_iterator(
        infix_const_iterator &&other) noexcept :
        _holder(std::move(other._holder)),
        _path(std::move(other._path)),
        _data(std::move(other._data)) {
  other._holder = nullptr;
  other._data = nullptr;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::infix_const_iterator::infix_const_iterator(
        binary_search_tree<tkey, tvalue> const* holder,
        typename binary_search_tree<tkey, tvalue>::node* subtree_root) : _holder(holder) {
  while (subtree_root != nullptr) {
    this->_path.push(subtree_root);
    subtree_root = subtree_root->left_subtree;
  }

  node* node = this->_path.empty() ? nullptr : this->_path.top();
  this->_data = holder->create_iterator_data(this->_path.size() - 1, node);
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_const_iterator::operator==(
        typename binary_search_tree<tkey, tvalue>::infix_const_iterator const &other) const noexcept {
  if (_path.empty() && other._path.empty()) {
    return true;
  }
  if (_path.empty() ^ other._path.empty()) {
    return false;
  }
  return _path.top() == other._path.top();
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_const_iterator::operator!=(
        typename binary_search_tree<tkey, tvalue>::infix_const_iterator const &other) const noexcept {
  return !(*this == other);
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::infix_const_iterator::pass_to_next_node() {
  auto &_path = this->_path;

  if (_path.empty()) {
    return; // UB
  }

  if (_path.top()->right_subtree != nullptr) {
    _path.push(_path.top()->right_subtree);
    while (_path.top()->left_subtree != nullptr) {
      _path.push(_path.top()->left_subtree);
    }
  } else {
    while (true) {
      node* current = _path.top();
      _path.pop();
      if (_path.empty()) {
        return;
      }
      if (_path.top()->left_subtree == current) {
        return;
      }
    }
  }
}


template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_iterator &
binary_search_tree<tkey, tvalue>::infix_const_iterator::operator++() {
  pass_to_next_node();

  if (!_path.empty()) {
    this->assign_data();
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_iterator const
binary_search_tree<tkey, tvalue>::infix_const_iterator::operator++(
        int not_used) {
  auto it = *this;
  ++*this;
  return it;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data*
binary_search_tree<tkey, tvalue>::infix_const_iterator::operator*() const {
  if (_path.empty()) {
    throw std::logic_error("Cannot get data from empty iterator");
  }

  return _data;
}

#pragma endregion infix_const_iterator implementation

#pragma region infix_reverse_iterator implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::infix_reverse_iterator::infix_reverse_iterator(
        infix_reverse_iterator const &other) :
        _holder(other._holder),
        _path(other._path) {
  if (_path.empty()) {
    _data = _holder->create_iterator_data();
  } else {
    _data = _holder->create_iterator_data(_path.size() - 1, _path.top());
  }
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::infix_reverse_iterator::infix_reverse_iterator(
        infix_reverse_iterator &&other) noexcept :
        _holder(std::move(other._holder)),
        _path(std::move(other._path)),
        _data(std::move(other._data)) {
  other._holder = nullptr;
  other._data = nullptr;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::infix_reverse_iterator::infix_reverse_iterator(
        binary_search_tree<tkey, tvalue> const* holder,
        typename binary_search_tree<tkey, tvalue>::node* subtree_root) : _holder(holder) {
  while (subtree_root != nullptr) {
    this->_state.push(subtree_root);
    subtree_root = subtree_root->right_subtree;
  }

  node* node = this->_state.empty() ? nullptr : this->_state.top();
  this->_data = holder->create_iterator_data(this->_state.size() - 1, node);
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator==(
        typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator const &other) const noexcept {
  if (_path.empty() && other._path.empty()) {
    return true;
  }
  if (_path.empty() ^ other._path.empty()) {
    return false;
  }
  return _path.top() == other._path.top();
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator!=(
        typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator const &other) const noexcept {
  return !(*this == other);
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::infix_reverse_iterator::pass_to_next_node() {
  auto &_path = this->_path;

  if (_path.empty()) {
    return; // UB
  }

  if (_path.top()->left_subtree != nullptr) {
    _path.push(_path.top()->left_subtree);
    while (_path.top()->right_subtree != nullptr) {
      _path.push(_path.top()->right_subtree);
    }
  } else {
    while (true) {
      node* current = _path.top();
      _path.pop();
      if (_path.empty()) {
        return;
      }
      if (_path.top()->right_subtree == current) {
        return;
      }
    }
  }
}


template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator &
binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator++() {
  pass_to_next_node();

  if (!_path.empty()) {
    this->assign_data();
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator const
binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator++(
        int not_used) {
  auto it = *this;
  ++*this;
  return it;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data*
binary_search_tree<tkey, tvalue>::infix_reverse_iterator::operator*() const {
  if (_path.empty()) {
    throw std::logic_error("Cannot get data from empty iterator");
  }

  return _data;
}

#pragma endregion infix_reverse_iterator implementation

#pragma region infix_const_reverse_iterator implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::infix_const_reverse_iterator(
        infix_const_reverse_iterator const &other) :
        _holder(other._holder),
        _path(other._path) {
  if (_path.empty()) {
    _data = _holder->create_iterator_data();
  } else {
    _data = _holder->create_iterator_data(_path.size() - 1, _path.top());
  }
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::infix_const_reverse_iterator(
        infix_const_reverse_iterator &&other) noexcept :
        _holder(std::move(other._holder)),
        _path(std::move(other._path)),
        _data(std::move(other._data)) {
  other._holder = nullptr;
  other._data = nullptr;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::infix_const_reverse_iterator(
        binary_search_tree<tkey, tvalue> const* holder,
        typename binary_search_tree<tkey, tvalue>::node* subtree_root) : _holder(holder) {
  while (subtree_root != nullptr) {
    this->_state.push(subtree_root);
    subtree_root = subtree_root->right_subtree;
  }

  node* node = this->_state.empty() ? nullptr : this->_state.top();
  this->_data = holder->create_iterator_data(this->_state.size() - 1, node);
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator==(
        typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator const &other) const noexcept {
  if (_path.empty() && other._path.empty()) {
    return true;
  }
  if (_path.empty() ^ other._path.empty()) {
    return false;
  }
  return _path.top() == other._path.top();
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator!=(
        typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator const &other) const noexcept {
  return !(*this == other);
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::pass_to_next_node() {
  auto &_path = this->_path;

  if (_path.empty()) {
    return; // UB
  }

  if (_path.top()->left_subtree != nullptr) {
    _path.push(_path.top()->left_subtree);
    while (_path.top()->right_subtree != nullptr) {
      _path.push(_path.top()->right_subtree);
    }
  } else {
    while (true) {
      node* current = _path.top();
      _path.pop();
      if (_path.empty()) {
        return;
      }
      if (_path.top()->right_subtree == current) {
        return;
      }
    }
  }
}


template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator &
binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator++() {
  pass_to_next_node();

  if (!_path.empty()) {
    this->assign_data();
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator const
binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator++(
        int not_used) {
  auto it = *this;
  ++*this;
  return it;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data*
binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator::operator*() const {
  if (_path.empty()) {
    throw std::logic_error("Cannot get data from empty iterator");
  }

  return _data;
}

#pragma endregion postfix_iterator implementation

#pragma region postfix_iterator implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_iterator::postfix_iterator(
        postfix_iterator const &other) :
        _holder(other._holder),
        _path(other._path) {
  if (_path.empty()) {
    _data = _holder->create_iterator_data();
  } else {
    _data = _holder->create_iterator_data(_path.size() - 1, _path.top());
  }
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_iterator::postfix_iterator(
        postfix_iterator &&other) noexcept :
        _holder(std::move(other._holder)),
        _path(std::move(other._path)),
        _data(std::move(other._data)) {
  other._holder = nullptr;
  other._data = nullptr;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_iterator::postfix_iterator(
        binary_search_tree<tkey, tvalue> const* holder,
        typename binary_search_tree<tkey, tvalue>::node* subtree_root) : _holder(holder) {
  while (subtree_root != nullptr) {
    this->_path.push(subtree_root);
    subtree_root = subtree_root->left_subtree != nullptr
                   ? subtree_root->left_subtree
                   : subtree_root->right_subtree;
  }

  node* node = this->_path.empty() ? nullptr : this->_path.top();
  this->_data = holder->create_iterator_data(this->_path.size() - 1, node);
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_iterator::operator==(
        typename binary_search_tree<tkey, tvalue>::postfix_iterator const &other) const noexcept {
  if (_path.empty() && other._path.empty()) {
    return true;
  }
  if (_path.empty() ^ other._path.empty()) {
    return false;
  }
  return _path.top() == other._path.top();
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_iterator::operator!=(
        typename binary_search_tree<tkey, tvalue>::postfix_iterator const &other) const noexcept {
  return !(*this == other);
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::postfix_iterator::pass_to_next_node() {
  auto &_path = this->_path;

  if (_path.empty()) {
    return; // UB
  }

  node* current = _path.top();
  _path.pop();

  if (_path.empty()) {
    return;
  }

  if (_path.top()->right_subtree == current || _path.top()->right_subtree == nullptr) {
    return;
  }

  current = _path.top()->right_subtree;

  while (current != nullptr) {
    this->_path.push(current);
    current = current->left_subtree != nullptr
              ? current->left_subtree
              : current->right_subtree;
  }
}


template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator &
binary_search_tree<tkey, tvalue>::postfix_iterator::operator++() {
  pass_to_next_node();

  if (!_path.empty()) {
    this->assign_data();
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator const
binary_search_tree<tkey, tvalue>::postfix_iterator::operator++(
        int not_used) {
  auto it = *this;
  ++*this;
  return it;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data*
binary_search_tree<tkey, tvalue>::postfix_iterator::operator*() const {
  if (_path.empty()) {
    throw std::logic_error("Cannot get data from empty iterator");
  }

  return _data;
}

#pragma endregion postfix_iterator implementation


#pragma region postfix_const_iterator implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_const_iterator::postfix_const_iterator(
        postfix_const_iterator const &other) :
        _holder(other._holder),
        _path(other._path) {
  if (_path.empty()) {
    _data = _holder->create_iterator_data();
  } else {
    _data = _holder->create_iterator_data(_path.size() - 1, _path.top());
  }
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_const_iterator::postfix_const_iterator(
        postfix_const_iterator &&other) noexcept :
        _holder(std::move(other._holder)),
        _path(std::move(other._path)),
        _data(std::move(other._data)) {
  other._holder = nullptr;
  other._data = nullptr;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_const_iterator::postfix_const_iterator(
        binary_search_tree<tkey, tvalue> const* holder,
        typename binary_search_tree<tkey, tvalue>::node* subtree_root) : _holder(holder) {
  while (subtree_root != nullptr) {
    this->_path.push(subtree_root);
    subtree_root = subtree_root->left_subtree != nullptr
                   ? subtree_root->left_subtree
                   : subtree_root->right_subtree;
  }

  node* node = this->_path.empty() ? nullptr : this->_path.top();
  this->_data = holder->create_iterator_data(this->_path.size() - 1, node);
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator==(
        typename binary_search_tree<tkey, tvalue>::postfix_const_iterator const &other) const noexcept {
  if (_path.empty() && other._path.empty()) {
    return true;
  }
  if (_path.empty() ^ other._path.empty()) {
    return false;
  }
  return _path.top() == other._path.top();
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator!=(
        typename binary_search_tree<tkey, tvalue>::postfix_const_iterator const &other) const noexcept {
  return !(*this == other);
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::postfix_const_iterator::pass_to_next_node() {
  auto &_path = this->_path;

  if (_path.empty()) {
    return; // UB
  }

  node* current = _path.top();
  _path.pop();

  if (_path.empty()) {
    return;
  }

  if (_path.top()->right_subtree == current || _path.top()->right_subtree == nullptr) {
    return;
  }

  current = _path.top()->right_subtree;

  while (current != nullptr) {
    this->_path.push(current);
    current = current->left_subtree != nullptr
              ? current->left_subtree
              : current->right_subtree;
  }
}


template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_iterator &
binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator++() {
  pass_to_next_node();

  if (!_path.empty()) {
    this->assign_data();
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_iterator const
binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator++(
        int not_used) {
  auto it = *this;
  ++*this;
  return it;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data*
binary_search_tree<tkey, tvalue>::postfix_const_iterator::operator*() const {
  if (_path.empty()) {
    throw std::logic_error("Cannot get data from empty iterator");
  }

  return _data;
}

#pragma endregion postfix_const_iterator implementation

#pragma region postfix_reverse_iterator implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::postfix_reverse_iterator(
        postfix_reverse_iterator const &other) :
        _holder(other._holder),
        _path(other._path) {
  if (_path.empty()) {
    _data = _holder->create_iterator_data();
  } else {
    _data = _holder->create_iterator_data(_path.size() - 1, _path.top());
  }
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::postfix_reverse_iterator(
        postfix_reverse_iterator &&other) noexcept :
        _holder(std::move(other._holder)),
        _path(std::move(other._path)),
        _data(std::move(other._data)) {
  other._holder = nullptr;
  other._data = nullptr;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::postfix_reverse_iterator(
        binary_search_tree<tkey, tvalue> const* holder,
        typename binary_search_tree<tkey, tvalue>::node* subtree_root) : _holder(holder) {
  while (subtree_root != nullptr) {
    this->_state.push(subtree_root);
    subtree_root = subtree_root->right_subtree != nullptr
                   ? subtree_root->right_subtree
                   : subtree_root->left_subtree;
  }

  node* node = this->_state.empty() ? nullptr : this->_state.top();
  this->_data = holder->create_iterator_data(this->_state.size() - 1, node);
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator==(
        typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator const &other) const noexcept {
  if (_path.empty() && other._path.empty()) {
    return true;
  }
  if (_path.empty() ^ other._path.empty()) {
    return false;
  }
  return _path.top() == other._path.top();
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator!=(
        typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator const &other) const noexcept {
  return !(*this == other);
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::pass_to_next_node() {
  auto &_path = this->_path;

  if (_path.empty()) {
    return; // UB
  }

  node* current = _path.top();
  _path.pop();

  if (_path.empty()) {
    return;
  }

  if (_path.top()->left_subtree == current || _path.top()->left_subtree == nullptr) {
    return;
  }

  current = _path.top()->left_subtree;

  while (current != nullptr) {
    this->_path.push(current);
    current = current->right_subtree != nullptr
              ? current->right_subtree
              : current->left_subtree;
  }
}


template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator &
binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator++() {
  pass_to_next_node();

  if (!_path.empty()) {
    this->assign_data();
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator const
binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator++(
        int not_used) {
  auto it = *this;
  ++*this;
  return it;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data*
binary_search_tree<tkey, tvalue>::postfix_reverse_iterator::operator*() const {
  if (_path.empty()) {
    throw std::logic_error("Cannot get data from empty iterator");
  }

  return _data;
}

#pragma endregion postfix_reverse_iterator implementation

#pragma region postfix_const_reverse_iterator implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(
        postfix_const_reverse_iterator const &other) :
        _holder(other._holder),
        _path(other._path) {
  if (_path.empty()) {
    _data = _holder->create_iterator_data();
  } else {
    _data = _holder->create_iterator_data(_path.size() - 1, _path.top());
  }
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(
        postfix_const_reverse_iterator &&other) noexcept :
        _holder(std::move(other._holder)),
        _path(std::move(other._path)),
        _data(std::move(other._data)) {
  other._holder = nullptr;
  other._data = nullptr;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(
        binary_search_tree<tkey, tvalue> const* holder,
        typename binary_search_tree<tkey, tvalue>::node* subtree_root) : _holder(holder) {
  while (subtree_root != nullptr) {
    this->_state.push(subtree_root);
    subtree_root = subtree_root->right_subtree != nullptr
                   ? subtree_root->right_subtree
                   : subtree_root->left_subtree;
  }

  node* node = this->_state.empty() ? nullptr : this->_state.top();
  this->_data = holder->create_iterator_data(this->_state.size() - 1, node);
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator==(
        typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator const &other) const noexcept {
  if (_path.empty() && other._path.empty()) {
    return true;
  }
  if (_path.empty() ^ other._path.empty()) {
    return false;
  }
  return _path.top() == other._path.top();
}

template<
        typename tkey,
        typename tvalue>
bool binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator!=(
        typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator const &other) const noexcept {
  return !(*this == other);
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::pass_to_next_node() {
  auto &_path = this->_path;

  if (_path.empty()) {
    return; // UB
  }

  node* current = _path.top();
  _path.pop();

  if (_path.empty()) {
    return;
  }

  if (_path.top()->left_subtree == current || _path.top()->left_subtree == nullptr) {
    return;
  }

  current = _path.top()->left_subtree;

  while (current != nullptr) {
    this->_path.push(current);
    current = current->right_subtree != nullptr
              ? current->right_subtree
              : current->left_subtree;
  }
}


template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator &
binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator++() {
  pass_to_next_node();

  if (!_path.empty()) {
    this->assign_data();
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator const
binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator++(
        int not_used) {
  auto it = *this;
  ++*this;
  return it;
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::iterator_data*
binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator::operator*() const {
  if (_path.empty()) {
    throw std::logic_error("Cannot get data from empty iterator");
  }

  return _data;
}

#pragma endregion postfix_const_reverse_iterator implementation

#pragma endregion iterators implementation

#pragma region target operations associated exception types implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_exception::insertion_of_existent_key_attempt_exception(
        tkey const &key):
        std::logic_error("Attempt to insert already existing key inside the tree.") {

}

template<
        typename tkey,
        typename tvalue>
tkey const &binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_exception::get_key() const noexcept {
  return _key;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::obtaining_of_nonexistent_key_attempt_exception::obtaining_of_nonexistent_key_attempt_exception(
        tkey const &key):
        std::logic_error("Attempt to obtain a value by non-existing key from the tree.") {

}

template<
        typename tkey,
        typename tvalue>
tkey const &binary_search_tree<tkey, tvalue>::obtaining_of_nonexistent_key_attempt_exception::get_key() const noexcept {
  return _key;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_exception::disposal_of_nonexistent_key_attempt_exception(
        tkey const &key):
        std::logic_error("Attempt to dispose a value by non-existing key from the tree.") {

}

template<
        typename tkey,
        typename tvalue>
tkey const &binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_exception::get_key() const noexcept {
  return _key;
}

#pragma endregion target operations associated exception types implementation

#pragma region template methods implementation

#pragma region binary_search_tree<tkey, tvalue>::template_method_basics implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::template_method_basics::template_method_basics(
        binary_search_tree<tkey, tvalue>* tree)
        : _tree(tree) {

}

#pragma endregion binary_search_tree<tkey, tvalue>::template_method_basics implementation

#pragma region search_tree<tkey, tvalue>::insertion_template_method implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::insertion_template_method::insertion_template_method(
        binary_search_tree<tkey, tvalue>* tree,
        typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy):
        binary_search_tree<tkey, tvalue>::template_method_basics::template_method_basics(tree),
        _insertion_strategy(insertion_strategy) {

}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::insertion_template_method::insert(
        tkey const &key,
        tvalue const &value) {
  auto path = this->find_path(key);
  if (*(path.top()) != nullptr) {
    switch (_insertion_strategy) {
      case insertion_of_existent_key_attempt_strategy::throw_an_exception:
        throw insertion_of_existent_key_attempt_exception(key);
      case insertion_of_existent_key_attempt_strategy::update_value:
        (*(path.top()))->value = value;
        break;
    }

    return;
  }

  this->_tree->call_node_constructor(
          *(path.top()) = reinterpret_cast<node*>(allocate_with_guard(this->_tree->get_node_size(), 1)), key, value);

  this->balance(path);
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::insertion_template_method::insert(
        tkey const &key,
        tvalue &&value) {
  auto path = this->find_path(key);
  if (*(path.top()) != nullptr) {
    switch (_insertion_strategy) {
      case insertion_of_existent_key_attempt_strategy::throw_an_exception:
        throw insertion_of_existent_key_attempt_exception(key);
      case insertion_of_existent_key_attempt_strategy::update_value:
        (*(path.top()))->value = std::move(value);
        break;
    }

    return;
  }

  this->_tree->call_node_constructor(
          *(path.top()) = reinterpret_cast<node*>(allocate_with_guard(this->_tree->get_node_size(), 1)), key,
          std::move(value));

  this->balance(path);
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::insertion_template_method::set_insertion_strategy(
        typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy) noexcept {
  _insertion_strategy = insertion_strategy;
}

template<
        typename tkey,
        typename tvalue>
allocator* binary_search_tree<tkey, tvalue>::insertion_template_method::get_allocator() const noexcept {
  return this->_tree->get_allocator();
}

template<
        typename tkey,
        typename tvalue>
logger* binary_search_tree<tkey, tvalue>::insertion_template_method::get_logger() const noexcept {
  return this->_tree->get_logger();
}

#pragma endregion search_tree<tkey, tvalue>::insertion_template_method implementation

#pragma region search_tree<tkey, tvalue>::obtaining_template_method implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::obtaining_template_method::obtaining_template_method(
        binary_search_tree<tkey, tvalue>* tree):
        binary_search_tree<tkey, tvalue>::template_method_basics::template_method_basics(tree) {

}

template<
        typename tkey,
        typename tvalue>
tvalue const &binary_search_tree<tkey, tvalue>::obtaining_template_method::obtain(
        tkey const &key) {
  auto path = this->find_path(key);
  if (*(path.top()) == nullptr) {
    throw obtaining_of_nonexistent_key_attempt_exception(key);
  }

  tvalue const &got_value = (*(path.top()))->value;

  this->balance(path);

  return got_value;
}

template<
        typename tkey,
        typename tvalue>
[[nodiscard]] allocator* binary_search_tree<tkey, tvalue>::obtaining_template_method::get_allocator() const noexcept {
  return this->_tree->get_allocator();
}

template<
        typename tkey,
        typename tvalue>
[[nodiscard]] logger* binary_search_tree<tkey, tvalue>::obtaining_template_method::get_logger() const noexcept {
  return this->_tree->get_logger();
}

#pragma endregion search_tree<tkey, tvalue>::obtaining_template_method implementation

#pragma region search_tree<tkey, tvalue>::disposal_template_method implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::disposal_template_method::disposal_template_method(
        binary_search_tree<tkey, tvalue>* tree,
        typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy):
        binary_search_tree<tkey, tvalue>::template_method_basics(tree) {

}

template<
        typename tkey,
        typename tvalue>
tvalue binary_search_tree<tkey, tvalue>::disposal_template_method::dispose(
        tkey const &key) {
  auto path = this->find_path(key);
  if (*(path.top()) == nullptr) {
    switch (_disposal_strategy) {
      case disposal_of_nonexistent_key_attempt_strategy::throw_an_exception:
        throw disposal_of_nonexistent_key_attempt_exception(key);
      case disposal_of_nonexistent_key_attempt_strategy::do_nothing:
        return tvalue();
    }
  }

  if ((*(path.top()))->left_subtree != nullptr && (*(path.top()))->right_subtree != nullptr) {
    auto* target_to_swap = *(path.top());
    auto** current = &((*(path.top()))->left_subtree);

    while (*current != nullptr) {
      path.push(current);
      current = &((*current)->right_subtree);
    }

    swap(std::move(target_to_swap->key), std::move((*(path.top()))->key));
    swap(std::move(target_to_swap->value), std::move((*(path.top()))->value));
  }

  tvalue value = std::move((*(path.top()))->value);
  node* leftover_subtree = (*(path.top()))->left_subtree == nullptr
                           ? (*(path.top()))->right_subtree
                           : (*(path.top()))->left_subtree;
  allocator::destruct(*(path.top()));
  deallocate_with_guard(*(path.top()));
  *(path.top()) = leftover_subtree;
  this->balance(path);
  return value;
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::disposal_template_method::set_disposal_strategy(
        typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy) noexcept {
  _disposal_strategy = disposal_strategy;
}

template<
        typename tkey,
        typename tvalue>
[[nodiscard]] inline allocator*
binary_search_tree<tkey, tvalue>::disposal_template_method::get_allocator() const noexcept {
  return this->_tree->get_allocator();
}

template<
        typename tkey,
        typename tvalue>
[[nodiscard]] logger* binary_search_tree<tkey, tvalue>::disposal_template_method::get_logger() const noexcept {
  return this->_tree->get_logger();
}

#pragma endregion search_tree<tkey, tvalue>::disposal_template_method implementation

#pragma endregion template methods

#pragma region construction, assignment, destruction implementation

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::binary_search_tree(
        typename binary_search_tree<tkey, tvalue>::insertion_template_method* insertion_template,
        typename binary_search_tree<tkey, tvalue>::obtaining_template_method* obtaining_template,
        typename binary_search_tree<tkey, tvalue>::disposal_template_method* disposal_template,
        std::function<int(tkey const &, tkey const &)> comparer,
        allocator* allocator,
        logger* logger):
        search_tree<tkey, tvalue>(comparer, logger, allocator),
        _root(nullptr),
        _insertion_template(insertion_template),
        _obtaining_template(obtaining_template),
        _disposal_template(disposal_template) {

}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::binary_search_tree(
        std::function<int(tkey const &, tkey const &)> keys_comparer,
        allocator* allocator,
        logger* logger,
        typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy,
        typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy):
        binary_search_tree(
                new binary_search_tree<tkey, tvalue>::insertion_template_method(this, insertion_strategy),
                new binary_search_tree<tkey, tvalue>::obtaining_template_method(this),
                new binary_search_tree<tkey, tvalue>::disposal_template_method(this, disposal_strategy),
                keys_comparer,
                allocator,
                logger) {

}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::binary_search_tree(
        binary_search_tree<tkey, tvalue> const &other):
        search_tree<tkey, tvalue>(other._keys_comparer, other.get_logger(), other.get_allocator()),
        _insertion_template(new binary_search_tree<tkey, tvalue>::insertion_template_method(this,
                                                                                            binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy::throw_an_exception)),
        _obtaining_template(new binary_search_tree<tkey, tvalue>::obtaining_template_method(this)),
        _disposal_template(new binary_search_tree<tkey, tvalue>::disposal_template_method(this,
                                                                                          binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy::throw_an_exception)) {
  _root = copy(other._root);
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::binary_search_tree(
        binary_search_tree<tkey, tvalue> &&other) noexcept:
        search_tree<tkey, tvalue>(other._keys_comparer, other.get_logger(), other.get_allocator()) {
  other._logger = nullptr;
  other._allocator = nullptr;

  _insertion_template = other._insertion_template;
  other._insertion_template = nullptr;

  _obtaining_template = other._obtaining_template;
  other._obtaining_template = nullptr;

  _disposal_template = other._disposal_template;
  other._disposal_template = nullptr;

  _root = other._root;
  other._root = nullptr;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue> &binary_search_tree<tkey, tvalue>::operator=(
        binary_search_tree<tkey, tvalue> const &other) {
  if (this != &other) {
    clear(_root);
    this->_allocator = other._allocator;
    this->_logger = other._logger;
    this->_keys_comparer = other._keys_comparer;
    _root = copy(other._root);
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue> &binary_search_tree<tkey, tvalue>::operator=(
        binary_search_tree<tkey, tvalue> &&other) noexcept {
  if (this != &other) {
    clear(_root);
    this->_keys_comparer = other._keys_comparer;

    this->_logger = other._logger;
    other._logger = nullptr;

    this->_allocator = other._allocator;
    other._allocator = nullptr;

    _insertion_template = other._insertion_template;
    other._insertion_template = nullptr;

    _obtaining_template = other._obtaining_template;
    other._obtaining_template = nullptr;

    _disposal_template = other._disposal_template;
    other._disposal_template = nullptr;

    _root = other._root;
    other._root = nullptr;
  }

  return *this;
}

template<
        typename tkey,
        typename tvalue>
binary_search_tree<tkey, tvalue>::~binary_search_tree() {
  delete _insertion_template;
  delete _obtaining_template;
  delete _disposal_template;
  clear(_root);
}

#pragma endregion construction, assignment, destruction implementation

#pragma region associative_container<tkey, tvalue> contract implementation

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::insert(
        tkey const &key,
        tvalue const &value) {
  _insertion_template->insert(key, value);
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::insert(
        tkey const &key,
        tvalue &&value) {
  _insertion_template->insert(key, std::move(value));
}

template<
        typename tkey,
        typename tvalue>
tvalue const &binary_search_tree<tkey, tvalue>::obtain(
        tkey const &key) {
  return _obtaining_template->obtain(key);
}

template<
        typename tkey,
        typename tvalue>
std::vector<typename associative_container<tkey, tvalue>::key_value_pair>
binary_search_tree<tkey, tvalue>::obtain_between(
        tkey const &lower_bound,
        tkey const &upper_bound,
        bool lower_bound_inclusive,
        bool upper_bound_inclusive) {
  return _obtaining_template->obtain_between(lower_bound, upper_bound, lower_bound_inclusive, upper_bound_inclusive);
}

template<
        typename tkey,
        typename tvalue>
tvalue binary_search_tree<tkey, tvalue>::dispose(
        tkey const &key) {
  return _disposal_template->dispose(key);
}

#pragma endregion associative_containers contract implementations

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::set_insertion_strategy(
        typename binary_search_tree<tkey, tvalue>::insertion_of_existent_key_attempt_strategy insertion_strategy) noexcept {
  this->trace_with_guard(
                  get_typename() + "::set_insertion_strategy(insertion_of_existent_key_attempt_strategy) : called.")
          ->debug_with_guard(
                  get_typename() + "::set_insertion_strategy(insertion_of_existent_key_attempt_strategy) : called.")
          ->debug_with_guard(get_typename() +
                             "::set_insertion_strategy(insertion_of_existent_key_attempt_strategy) : insertion strategy set to "
                             + (insertion_strategy == insertion_of_existent_key_attempt_strategy::update_value
                                ? "update_value" : "throw_an_exception"));

  _insertion_template->_insertion_strategy = insertion_strategy;

  this->trace_with_guard(
                  get_typename() + "::set_insertion_strategy(insertion_of_existent_key_attempt_strategy) : called.")
          ->debug_with_guard(
                  get_typename() + "::set_insertion_strategy(insertion_of_existent_key_attempt_strategy) : called.")
          ->debug_with_guard(get_typename() +
                             "::set_insertion_strategy(insertion_of_existent_key_attempt_strategy) : insertion strategy set to "
                             + (insertion_strategy == insertion_of_existent_key_attempt_strategy::update_value
                                ? "update_value" : "throw_an_exception"));
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::set_removal_strategy(
        typename binary_search_tree<tkey, tvalue>::disposal_of_nonexistent_key_attempt_strategy disposal_strategy) noexcept {
  this->trace_with_guard(
                  get_typename() + "::set_disposal_strategy(disposal_of_nonexistent_key_attempt_strategy) : called.")
          ->debug_with_guard(
                  get_typename() + "::set_disposal_strategy(disposal_of_nonexistent_key_attempt_strategy) : called.")
          ->debug_with_guard(get_typename() +
                             "::set_disposal_strategy(disposal_of_nonexistent_key_attempt_strategy) : disposal strategy set to "
                             + (disposal_strategy == disposal_of_nonexistent_key_attempt_strategy::update_value
                                ? "update_value" : "throw_an_exception"));

  _disposal_template = disposal_strategy;

  this->trace_with_guard(get_typename() +
                         "::set_disposal_strategy(disposal_of_nonexistent_key_attempt_strategy) : successfuly finished.")
          ->debug_with_guard(get_typename() +
                             "::set_disposal_strategy(disposal_of_nonexistent_key_attempt_strategy) : successfuly finished.");
}

#pragma region iterators requesting implementation

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator
binary_search_tree<tkey, tvalue>::begin_prefix() const noexcept {
  return binary_search_tree<tkey, tvalue>::prefix_iterator(this, reinterpret_cast<node*>(this->_root));
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_iterator
binary_search_tree<tkey, tvalue>::end_prefix() const noexcept {
  return binary_search_tree<tkey, tvalue>::prefix_iterator(this, nullptr);
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_iterator
binary_search_tree<tkey, tvalue>::cbegin_prefix() const noexcept {
  return binary_search_tree<tkey, tvalue>::prefix_const_iterator(this, reinterpret_cast<node*>(this->_root));
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_iterator
binary_search_tree<tkey, tvalue>::cend_prefix() const noexcept {
  return binary_search_tree<tkey, tvalue>::prefix_const_iterator(this, nullptr);
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator
binary_search_tree<tkey, tvalue>::rbegin_prefix() const noexcept {
  return binary_search_tree<tkey, tvalue>::prefix_reverse_iterator(this, reinterpret_cast<node*>(this->_root));
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_reverse_iterator
binary_search_tree<tkey, tvalue>::rend_prefix() const noexcept {
  return binary_search_tree<tkey, tvalue>::prefix_reverse_iterator(this, nullptr);
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue>::crbegin_prefix() const noexcept {
  return binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator(this, reinterpret_cast<node*>(this->_root));
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue>::crend_prefix() const noexcept {
  return binary_search_tree<tkey, tvalue>::prefix_const_reverse_iterator(this, nullptr);
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator
binary_search_tree<tkey, tvalue>::begin_infix() const noexcept {
  return binary_search_tree<tkey, tvalue>::infix_iterator(this, reinterpret_cast<node*>(this->_root));
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_iterator binary_search_tree<tkey, tvalue>::end_infix() const noexcept {
  return binary_search_tree<tkey, tvalue>::infix_iterator(this, nullptr);
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_iterator
binary_search_tree<tkey, tvalue>::cbegin_infix() const noexcept {
  return binary_search_tree<tkey, tvalue>::infix_const_iterator(this, reinterpret_cast<node*>(this->_root));
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_iterator
binary_search_tree<tkey, tvalue>::cend_infix() const noexcept {
  return binary_search_tree<tkey, tvalue>::infix_const_iterator(this, nullptr);
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator
binary_search_tree<tkey, tvalue>::rbegin_infix() const noexcept {
  return binary_search_tree<tkey, tvalue>::infix_reverse_iterator(this, reinterpret_cast<node*>(this->_root));
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_reverse_iterator
binary_search_tree<tkey, tvalue>::rend_infix() const noexcept {
  return binary_search_tree<tkey, tvalue>::infix_reverse_iterator(this, nullptr);
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue>::crbegin_infix() const noexcept {
  return binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator(this, reinterpret_cast<node*>(this->_root));
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue>::crend_infix() const noexcept {
  return binary_search_tree<tkey, tvalue>::infix_const_reverse_iterator(this, nullptr);
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator
binary_search_tree<tkey, tvalue>::begin_postfix() const noexcept {
  return binary_search_tree<tkey, tvalue>::postfix_iterator(this, reinterpret_cast<node*>(this->_root));
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_iterator
binary_search_tree<tkey, tvalue>::end_postfix() const noexcept {
  return binary_search_tree<tkey, tvalue>::postfix_iterator(this, nullptr);
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_iterator
binary_search_tree<tkey, tvalue>::cbegin_postfix() const noexcept {
  return binary_search_tree<tkey, tvalue>::postfix_const_iterator(this, reinterpret_cast<node*>(this->_root));
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_iterator
binary_search_tree<tkey, tvalue>::cend_postfix() const noexcept {
  return binary_search_tree<tkey, tvalue>::postfix_const_iterator(this, nullptr);
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator
binary_search_tree<tkey, tvalue>::rbegin_postfix() const noexcept {
  return binary_search_tree<tkey, tvalue>::postfix_reverse_iterator(this, reinterpret_cast<node*>(this->_root));
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_reverse_iterator
binary_search_tree<tkey, tvalue>::rend_postfix() const noexcept {
  return binary_search_tree<tkey, tvalue>::postfix_reverse_iterator(this, nullptr);
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue>::crbegin_postfix() const noexcept {
  return binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator(this, reinterpret_cast<node*>(this->_root));
}

template<
        typename tkey,
        typename tvalue>
typename binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue>::crend_postfix() const noexcept {
  return binary_search_tree<tkey, tvalue>::postfix_const_reverse_iterator(this, nullptr);
}

#pragma endregion iterators request implementation

#pragma region subtree rotations implementation

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::small_left_rotation(
        binary_search_tree<tkey, tvalue>::node*&subtree_root,
        bool validate) const {
  this->trace_with_guard(get_typename() + "::small_left_rotation() : called.")
          ->debug_with_guard(get_typename() + "::small_left_rotation() : called.")
          ->debug_with_guard(get_typename() + "::small_left_rotation() : rotating node with key \"" +
                             std::to_string(subtree_root->key) + "\"");

  if (validate && (subtree_root == nullptr || subtree_root->right_subtree == nullptr)) {
    throw std::logic_error("Cannot perform small left rotation");
  }

  binary_search_tree<tkey, tvalue>::node* new_subtree_root = subtree_root->right_subtree;

  subtree_root->right_subtree = new_subtree_root->left_subtree;
  new_subtree_root->left_subtree = subtree_root;
  subtree_root = new_subtree_root;

  update_node_data(new_subtree_root->left_subtree);
  update_node_data(new_subtree_root);

  this->trace_with_guard(get_typename() + "::small_left_rotation() : successfuly finished.")
          ->debug_with_guard(get_typename() + "::small_left_rotation() : successfuly finished.");

}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::small_right_rotation(
        binary_search_tree<tkey, tvalue>::node*&subtree_root,
        bool validate) const {
  this->trace_with_guard(get_typename() + "::small_right_rotation() : called.")
          ->debug_with_guard(get_typename() + "::small_right_rotation() : called.")
          ->debug_with_guard(get_typename() + "::small_right_rotation() : rotating node with key \"" +
                             std::to_string(subtree_root->key) + "\"");

  if (validate && (subtree_root == nullptr || subtree_root->left_subtree == nullptr)) {
    throw std::logic_error("Cannot perform small right rotation");
  }

  binary_search_tree<tkey, tvalue>::node* new_subtree_root = subtree_root->left_subtree;

  subtree_root->left_subtree = new_subtree_root->right_subtree;
  new_subtree_root->right_subtree = subtree_root;
  subtree_root = new_subtree_root;

  update_node_data(new_subtree_root->right_subtree);
  update_node_data(new_subtree_root);

  this->trace_with_guard(get_typename() + "::small_right_rotation() : successfuly finished.")
          ->debug_with_guard(get_typename() + "::small_right_rotation() : successfuly finished.");
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::big_left_rotation(
        binary_search_tree<tkey, tvalue>::node*&subtree_root,
        bool validate) const {
  this->trace_with_guard(get_typename() + "::big_left_rotation() : called.")
          ->debug_with_guard(get_typename() + "::big_left_rotation() : called.");

  if (validate && (subtree_root == nullptr ||
                   subtree_root->right_subtree == nullptr || subtree_root->right_subtree->left_subtree == nullptr)) {
    throw std::logic_error("Cannot perform big left rotation");
  }

  small_right_rotation(subtree_root->right_subtree, false);
  small_left_rotation(subtree_root, false);

  this->trace_with_guard(get_typename() + "::big_left_rotation() : successfuly finished.")
          ->debug_with_guard(get_typename() + "::big_left_rotation() : successfuly finished.");
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::big_right_rotation(
        binary_search_tree<tkey, tvalue>::node*&subtree_root,
        bool validate) const {
  this->trace_with_guard(get_typename() + "::big_right_rotation() : called.")
          ->debug_with_guard(get_typename() + "::big_right_rotation() : called.");

  if (validate && (subtree_root == nullptr ||
                   subtree_root->left_subtree == nullptr || subtree_root->left_subtree->right_subtree == nullptr)) {
    throw std::logic_error("Cannot perform big right rotation");
  }

  small_left_rotation(subtree_root->left_subtree, false);
  small_right_rotation(subtree_root, false);

  this->trace_with_guard(get_typename() + "::big_right_rotation() : successfuly finished.")
          ->debug_with_guard(get_typename() + "::big_right_rotation() : successfuly finished.");
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::double_left_rotation(
        binary_search_tree<tkey, tvalue>::node*&subtree_root,
        bool at_grandparent_first,
        bool validate) const {
  this->trace_with_guard(get_typename() + "::double_left_rotation() : called.")
          ->debug_with_guard(get_typename() + "::double_left_rotation() : called.");

  if (validate && (subtree_root == nullptr ||
                   subtree_root->right_subtree == nullptr || subtree_root->right_subtree->right_subtree == nullptr)) {
    throw std::logic_error("Cannot perform double right rotation");
  }

  if (at_grandparent_first) {
    small_left_rotation(subtree_root, false);
    small_left_rotation(subtree_root, false);
  } else {
    small_left_rotation(subtree_root->right, false);
    small_left_rotation(subtree_root, false);
  }

  this->trace_with_guard(get_typename() + "::double_left_rotation() : successfuly finished.")
          ->debug_with_guard(get_typename() + "::double_left_rotation() : successfuly finished.");
}

template<
        typename tkey,
        typename tvalue>
void binary_search_tree<tkey, tvalue>::double_right_rotation(
        binary_search_tree<tkey, tvalue>::node*&subtree_root,
        bool at_grandparent_first,
        bool validate) const {
  this->trace_with_guard(get_typename() + "::double_right_rotation() : called.")
          ->debug_with_guard(get_typename() + "::double_right_rotation() : called.");

  if (validate && (subtree_root == nullptr ||
                   subtree_root->left_subtree == nullptr || subtree_root->left_subtree->left_subtree == nullptr)) {
    throw std::logic_error("Cannot perform double left rotation");
  }

  if (at_grandparent_first) {
    small_left_rotation(subtree_root, false);
    small_left_rotation(subtree_root, false);
  } else {
    small_left_rotation(subtree_root->left, false);
    small_left_rotation(subtree_root, false);
  }

  this->trace_with_guard(get_typename() + "::double_right_rotation() : successfuly finished.")
          ->debug_with_guard(get_typename() + "::double_right_rotation() : successfuly finished.");
}

#pragma endregion subtree rotations implementation

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H