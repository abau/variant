#include <cassert>
#include <type_traits>

// `VariantDetails` encapsulates internal details of the variant implementation
namespace VariantDetails {

  template <typename ...>
  union VariantUnion;

  // Utility structure to set value in a union
  template <unsigned int, typename, typename ...>
  struct SetValue;

  template <typename U, typename T, typename ... Ts>
  struct SetValue <0,U,T,Ts ...> {
    static void run (VariantUnion <T, Ts ...>* variant, const U& u) {
      static_assert (std::is_same <T,U>::value, "variant type mismatch");
      variant->t = new U (u);
    }
  };

  template <unsigned int i, typename U, typename T, typename ... Ts>
  struct SetValue <i,U,T,Ts ...> {
    static void run (VariantUnion <T,Ts ...>* variant, const U& u) {
      SetValue <i-1,U,Ts ...> :: run (&variant->ts, u);
    }
  };

  // Utility structure to get the type of the i-th variant
  template <unsigned int, typename ...>
  struct GetType;

  template <typename T, typename ... Ts>
  struct GetType <0,T,Ts ...> {
    typedef T type;
  };

  template <unsigned int i, typename T, typename ... Ts>
  struct GetType <i,T,Ts ...> {
    typedef typename GetType <i-1,Ts ...>::type type;
  };

  // Utility structure to get the index of the variant of a certain type
  template <unsigned int,typename, typename ...>
  struct GetIndex;

  template <unsigned int i, typename U, typename T>
  struct GetIndex <i,U,T> {
    static constexpr bool         found = std::is_same <T,U>::value;
    static constexpr unsigned int index = i;
  };

  template <unsigned int i, typename U, typename T, typename ... Ts>
  struct GetIndex <i,U,T,Ts ...> {
    static constexpr bool found = 
      std::is_same <T,U>::value ? true : GetIndex <i+1,U,Ts ...>::found;

    static constexpr unsigned int index =
      std::is_same <T,U>::value ? i : GetIndex <i+1,U,Ts ...>::index;
  };

  // `VariantUnion <T1,T2,...,Tn>` implements union over pointers of type `T1,T2,...,Tn`
  //
  // Case n == 1;
  template <typename T>
  union VariantUnion <T> {
    T* t;

    VariantUnion () : t (nullptr) {}
    VariantUnion (const VariantUnion&) = delete;

    const VariantUnion& operator= (const VariantUnion&) = delete;

    void copy (unsigned int i, const VariantUnion& other) {
      assert (i == 0);
      this->t = new T (*other.t);
    }

    void release (unsigned int i) {
      assert (i == 0);
      delete this->t;
    }

    template <unsigned int i, typename U>
    void set (const U& u) {
      SetValue <i,U,T> :: run (this, u);
    }

    template <typename U>
    U* get (unsigned int i) const {
      assert (i == 0);
      assert ((std::is_same <T,U>::value));
      return reinterpret_cast <U*> (this->t);
    }
  };

  // Case n > 1;
  template <typename T,typename ... Ts>
  union VariantUnion <T, Ts ...> {
    T* t;
    VariantUnion <Ts ...> ts;

    VariantUnion () : t (nullptr) {}
    VariantUnion (const VariantUnion&) = delete;

    const VariantUnion& operator= (const VariantUnion&) = delete;

    void copy (unsigned int i, const VariantUnion& other) {
      if (i == 0)
        this->t = new T (*other.t);
      else
        this->ts.copy (i-1, other.ts);
    }

    void release (unsigned int i) {
      if (i == 0)
        delete this->t;
      else
        this->ts.release (i-1);
    }

    template <unsigned int i, typename U>
    void set (const U& u) {
      SetValue <i,U,T,Ts ...> :: run (this, u);
    }

    template <typename U>
    U* get (unsigned int i) const {
      if (i == 0) {
        assert ((std::is_same<U,T>::value));
        return reinterpret_cast <U*> (this->t);
      }
      else
        return this->ts.get <U> (i-1);
    }
  };
};

// `Variant` wraps `VariantUnion` and handles construction, access and destruction
template <typename ... Ts>
class Variant {
  public:
    Variant () 
      : _varUnion ()
      , _isSet    (false)
      , _setTo    (0)
      {}

    Variant (const Variant& other) 
      : _varUnion ()
      , _isSet    (other._isSet)
      , _setTo    (other._setTo) {

        if (this->_isSet)
          this->_varUnion.copy (this->_setTo, other._varUnion);
    }

    const Variant& operator= (const Variant& other) {
      if (this == &other) {
        return *this;
      }
      this->release ();
      this->_isSet = other._isSet;
      this->_setTo = other._setTo;

      if (this->_isSet) {
        this->_varUnion.copy (this->_setTo, other._varUnion);
      }
    }

    ~Variant () {
      this->release ();
    }

    void release () {
      if (this->_isSet) {
        this->_varUnion.release (this->_setTo);
        this->_isSet = false;
      }
    }

    template <typename U>
    U* get () const {
      assert (this->_isSet);
      return this->_varUnion.template get <U> (this->_setTo);
    }

    template <unsigned int i, typename U = VariantDetails::GetType <i,Ts ...> >
    void setAt (const U& u) {
      if (this->_isSet) {
        this->release ();
      }
      this->_varUnion.template set <i,U> (u);
      this->_isSet = true;
      this->_setTo = i;
    }

    template <typename U>
    void set (const U& u) {
      constexpr bool         found = VariantDetails::GetIndex <0,U,Ts ...>::found;
      constexpr unsigned int index = VariantDetails::GetIndex <0,U,Ts ...>::index;
      static_assert (found, "variant type not found");
      this->template setAt <index,U> (u);
    }

    template <typename U>
    bool is () const { 
      assert (this->_isSet);
      constexpr bool         found = VariantDetails::GetIndex <0,U,Ts ...>::found;
      constexpr unsigned int index = VariantDetails::GetIndex <0,U,Ts ...>::index;
      static_assert (found, "variant type not found");
      return this->_setTo == index; 
    }

  private:
    VariantDetails::VariantUnion <Ts ...> _varUnion;
    bool                                  _isSet;
    unsigned int                          _setTo;
};
