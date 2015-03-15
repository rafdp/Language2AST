

struct NodeContent_t
{
    uint8_t flag;
    double data;

    NodeContent_t (uint8_t flag_, double data_ = 0) :
        flag (flag_),
        data (data_)
    {}

    NodeContent_t () :
        flag (0),
        data (0)
    {}

/*
    void Print ()
    {
        switch (flag)
        {
        case FLAG_NUMERIC:
            //printf ("|num %I64u|", data);
            if (*(reinterpret_cast<int64_t*> (&data)) < 0)
                printf ("%lld", data);
            else
                printf ("%llu", data);
            break;
        case FLAG_NUMERICD:
            //printf ("|num %I64u|", data);
            printf ("%g", *(reinterpret_cast<double*> (&data)));
            break;
        case FLAG_OPERATOR:
            //printf ("|op \'%c\'|", static_cast <uint8_t> (data));
            printf ("%c", static_cast <uint8_t> (data));
            break;
        case FLAG_FUNCTION:
            printf ("|func %llu|", data);
            break;
        case FLAG_ARGUMENT:
            //printf ("|arg %I64u|", data);
            printf ("x");
            break;
        default:
            printf ("|unk %d %llu|", flag, data);
            break;
        }
    }

    double GetDouble ()
    {
        switch (flag)
        {
        case FLAG_NUMERIC:
            //printf ("|num %I64u|", data);
            return static_cast<double> (data);
            break;
        case FLAG_NUMERICD:
            //printf ("|num %I64u|", data);
            return *(reinterpret_cast<double*> (&data));
            break;
        default:
            return 0;
            break;
        }
    }

    bool ResolvePriorities (const NodeContent_t& elem)
    {
        if (flag == FLAG_FUNCTION)
        {
            if (flag == FLAG_OPERATOR) return true;
            else return true;
        }
        else
        if ((elem.flag == FLAG_NUMERIC && *(reinterpret_cast<const int64_t*> (&(elem.data))) < 0) ||
            (elem.flag == FLAG_NUMERICD && *(reinterpret_cast<const double*> (&(elem.data))) < 0)) return true;
        else
        if (elem.flag == FLAG_NUMERIC ||
            elem.flag == FLAG_NUMERICD ||
            elem.flag == FLAG_ARGUMENT ||
            elem.flag == FLAG_FUNCTION) return false;
        else
        if (flag == FLAG_OPERATOR)
        {
            if (data == OP_PLUS || data == OP_MINUS) return false;
            if (data == OP_POW) return true;
            if ((data == OP_MUL || data == OP_DIV) && (elem.data == OP_PLUS || elem.data == OP_MINUS)) return true;
        }
        return false;
    }
*/
};


template <class Element_t>
class Node_t : NZA_t
{

    DISABLE_CLASS_COPY (Node_t)

    std::vector<Node_t*> children_;
    Node_t* parent_;
    Element_t elem_;

    void ok ()
    {
        DEFAULT_OK_BLOCK
    }
public:

    Node_t (const Element_t& elem = Element_t ())
    try :
        children_   (),
        parent_     (),
        elem_       (elem)
    {
    END (CTOR)

    Node_t (Node_t* parent,
            const Element_t& elem = ZERO (Element_t))
    try :
        children_   (),
        parent_     (parent),
        elem_       (elem)
    {
    END (CTOR)

    template <typename T = Element_t>
    Node_t<Element_t>* PushChild (T elem = T())
    {
        BEGIN

        Node_t<Element_t>* ptr = new Node_t (this, elem);

        children_.push_back (ptr);

        return ptr;

        END (PUSH_CHILD)
    }


    void SetChild (uint32_t n, Node_t* child)
    {
        BEGIN
        if (n > children_.size ()) return;
        if (children_[n] != nullptr)
        {
            delete children_[n];
            children_[n] = nullptr;
        }
        children_[n] = child;
        if (child) children_[n]->parent_ = this;
        END (SET_CHILD)
    }


    bool Leaf ()
    {
        BEGIN
        return (children_.empty ());
        END (LEAF)
    }


    Node_t* GetChild (uint32_t n)
    {
        BEGIN
        if (n > children_.size ())
            _EXC_N (OUT_OF_RANGE, "Trying to access out of range element")
        return children_[n];
        END (GET_CHILD)
    }


    Node_t* GetLastChild ()
    {
        BEGIN
        if (children_.size () == 0)
            _EXC_N (NO_LAST_CHILD, "No last child")
        return *children_.rbegin ();
        END (GET_LAST_CHILD)
    }

    Node_t& operator [] (uint32_t n)
    {
        BEGIN
        return *GetChild (n);
        END (OP_SQUARE_BRACKETS)
    }

    Node_t* GetParent ()
    {
        BEGIN
        return parent_;
        END (GET_PARENT)
    }



    bool Root ()
    {
        BEGIN
        return parent_ == nullptr;
        END (ROOT)
    }


    ~Node_t ()
    {
        BEGIN
        Clear ();
        if (parent_)
        {
            auto found = std::find (parent_->children_.begin(),
                                    parent_->children_.end (),
                                    this);
            if (found != parent_->children_.end ())
                *found = nullptr;
        }
        return;
        END (DTOR)
    }


    void SetElem (const Element_t& elem)
    {
        BEGIN
        elem_ = elem;
        END (SET_ELEM)
    }


    Element_t& GetElem ()
    {
        BEGIN
        return elem_;
        END (GET_ELEM)
    }

    void Clear ()
    {
        BEGIN
        for (auto& i : children_)
        {
            if (i)
            {
                delete i;
                i = nullptr;
            }
        }
        children_.clear ();
        END (CLEAR)
    }

/*
    void DumpPrefix ()
    {
        printf ("(");
        elem_.Print();
        if (leftChild_)  {printf (" "); leftChild_->DumpPrefix ();}
        if (rightChild_) rightChild_->DumpPrefix ();
        printf (")");
    }

    void DumpInfix ()
    {
        if (leftChild_)
        {
            if (elem_.ResolvePriorities (leftChild_->GetElem()))
            {
                printf ("(");
                leftChild_->DumpInfix ();
                printf (")");
            }
            else leftChild_->DumpInfix ();
        }
        elem_.Print();

        if (rightChild_)
        {
            if (elem_.ResolvePriorities (rightChild_->GetElem()))
            {
                printf ("(");
                rightChild_->DumpInfix ();
                printf (")");
            }
            else rightChild_->DumpInfix ();
        }
    }

    void DumpPostfix ()
    {
        printf ("(");
        if (leftChild_)  leftChild_->DumpPostfix ();
        if (rightChild_) {rightChild_->DumpPostfix (); printf (" ");}
        if (leftChild_ && !rightChild_) printf (" ");
        elem_.Print();
        printf (")");
    }
*/
/*
    Node_t* UnlinkRightChild ()
    BEGIN
        Node_t* ret = rightChild_;
        rightChild_ = nullptr;
        return ret;
    END (ERROR_UNLINK_RIGHT)

    Node_t* UnlinkLeftChild ()
    BEGIN
        Node_t* ret = leftChild_;
        leftChild_ = nullptr;
        return ret;
    END (ERROR_UNLINK_LEFT)
*/

};
