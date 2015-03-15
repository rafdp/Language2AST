
class LolcodeLexicalAnalyzer_t : NZA_t
{
    DISABLE_CLASS_COPY (LolcodeLexicalAnalyzer_t)


    StringTable_t                vars_;
    StringTable_t                funcs_;
    VirtualCodeRepresentation_t* code_;
    std::vector<uint32_t>        relink_;
    CumulativeErrors_t*          cerrs_;

    Node_t<NodeContent_t>        treeRoot_;

    void ok ();
public:
    LolcodeLexicalAnalyzer_t (VirtualCodeRepresentation_t* code,
                              CumulativeErrors_t* cerrs);

    ~LolcodeLexicalAnalyzer_t () {}

    void Process ();

    void ArithmeticParser (Node_t<NodeContent_t>* currentNode,
                           std::vector<Token_t>::iterator& currentSource);

    void RecursiveAnalyzer (Node_t<NodeContent_t>* currentNode,
                            std::vector<Token_t>::iterator& currentSource,
                            Node_t<NodeContent_t>* currentFunction);

    void AddError (const Token_t& tok,
                   std::string msg,
                   uint32_t mode = EM_ERROR);

};

void LolcodeLexicalAnalyzer_t::AddError (const Token_t& tok,
                                         std::string msg,
                                         uint32_t mode)
{
    cerrs_->AddError (code_->program_[tok.line],
                      msg,
                      tok.line + 1,
                      tok.shift,
                      mode);
}

void LolcodeLexicalAnalyzer_t::ok ()
{
    DEFAULT_OK_BLOCK
    if (code_ == nullptr)
        _EXC_N (NULL_CODE_PTR, "Null code pointer")
    if (cerrs_ == nullptr)
        _EXC_N (NULL_CUMLATIVE_ERRORS_PTR, "Null cumulative errors pointer")
}

LolcodeLexicalAnalyzer_t::LolcodeLexicalAnalyzer_t (VirtualCodeRepresentation_t* code,
                                                    CumulativeErrors_t* cerrs)
try :
    vars_       (),
    funcs_      (),
    code_       (code),
    relink_     (),
    cerrs_      (cerrs),
    treeRoot_   (NodeContent_t (NODE_ROOT, 0.0))
{
/*
}
*/

END (CTOR)


void LolcodeLexicalAnalyzer_t::Process ()
{
    BEGIN
    treeRoot_.Clear ();

    treeRoot_.PushChild (NodeContent_t(NODE_VAR_INIT));

    funcs_.Register ("NullFunc");

    relink_.assign (code_->types_.size (), 0);

    for (auto iter = code_->types_.begin ();
         iter < code_->types_.end();
         iter++)
    {
        if (*iter == TOKEN_VAR)
        {
            uint32_t newVarIndex = vars_.Register (code_->GetString (iter - code_->types_.begin ()));
            treeRoot_[0].PushChild (NodeContent_t(NODE_VARIABLE, newVarIndex));
            relink_[iter - code_->types_.begin ()] = newVarIndex;
        }

        if (*iter == TOKEN_FUNC)
        {
            uint32_t newFuncIndex = funcs_.Register (code_->GetString (iter - code_->types_.begin ()));
            treeRoot_.PushChild (NodeContent_t(NODE_USER_FUNCTION, newFuncIndex));
            relink_[iter - code_->types_.begin ()] = newFuncIndex;
        }
    }

    funcs_.Register ("Main");
    treeRoot_.PushChild (NodeContent_t (NODE_MAIN, 0));

    //! run recursive analyzer

    auto currentSource = code_->tokens_.begin ();
    auto currentNode = &treeRoot_;

    RecursiveAnalyzer (currentNode, currentSource, nullptr);

    END (PROCESS)
}

void LolcodeLexicalAnalyzer_t::ArithmeticParser (Node_t<NodeContent_t>* currentNode,
                                                 std::vector<Token_t>::iterator& currentSource)
{
    BEGIN
    #define CASE_END \
        currentSource++; \
        ArithmeticParser (currentNode->PushChild (), currentSource); \
        if (currentNode->GetNChildren () != 2) AddError (*old, "Invalid amount ov arguments");
    auto old = currentSource;
    switch (currentSource->type)
    {
        case TOKEN_BOTH_SAEM:
            currentNode->SetElem (NodeContent_t (NODE_OPERATOR, OP_BOOL_EQUAL));
            CASE_END
            break;
        case TOKEN_DIFFRINT:
            currentNode->SetElem (NodeContent_t (NODE_OPERATOR, OP_BOOL_NOT_EQUAL));
            CASE_END
            break;
        case TOKEN_SUM_OF:
            currentNode->SetElem (NodeContent_t (NODE_OPERATOR, OP_PLUS));
            CASE_END
            break;
        case TOKEN_DIFF_OF:
            currentNode->SetElem (NodeContent_t (NODE_OPERATOR, OP_MINUS));
            CASE_END
            break;
        case TOKEN_PRODUKT_OF:
            currentNode->SetElem (NodeContent_t (NODE_OPERATOR, OP_MULTIPLY));
            CASE_END
            break;
        case TOKEN_QUOSHUNT_OF:
            currentNode->SetElem (NodeContent_t (NODE_OPERATOR, OP_DIVIDE));
            CASE_END
            break;
        case TOKEN_BIGGR_OF:
            currentNode->SetElem (NodeContent_t (NODE_STD_FUNCTION, STD_FUNC_MAX));
            CASE_END
            break;
        case TOKEN_SMALLR_OF:
            currentNode->SetElem (NodeContent_t (NODE_STD_FUNCTION, STD_FUNC_MAX));
            CASE_END
            break;
        case TOKEN_EITHER_OF:
            currentNode->SetElem (NodeContent_t (NODE_OPERATOR, OP_BOOL_OR));
            CASE_END
            break;
        case TOKEN_BOTH_OF:
            currentNode->SetElem (NodeContent_t (NODE_OPERATOR, OP_BOOL_AND));
            CASE_END
            break;
        case TOKEN_WON_OF:
            currentNode->SetElem (NodeContent_t (NODE_OPERATOR, OP_BOOL_OR));
            CASE_END
            currentNode->InsertAndSlide (0, NodeContent_t (NODE_OPERATOR, OP_BOOL_AND));
            currentNode[0].PushChildren (NodeContent_t (NODE_OPERATOR, OP_NOT));
            currentNode[0][1].PushChildren ();

            currentNode->InsertAndSlide (1, NodeContent_t (NODE_OPERATOR, OP_BOOL_AND));
            currentNode[1].PushChildren (NodeContent_t (NODE_OPERATOR, OP_NOT));
            currentNode[1][1].PushChildren ();

            CopyTree (&currentNode[0][0], &currentNode[1][1][0]);
            CopyTree (&currentNode[1][0], &currentNode[0][1][0]);

            break;

        case TOKEN_AN:
            currentNode = currentNode->GetParent ();
            currentSource++;
            ArithmeticParser (currentNode->PushChild (), currentSource);
            break;

        case TOKEN_NOT:
            currentNode->SetElem (NodeContent_t (NODE_OPERATOR, OP_NOT));
            currentSource++;
            ArithmeticParser (currentNode->PushChild (), currentSource);
            break;

        case TOKEN_VAR:
        {
            currentNode->SetElem (NodeContent_t (NODE_VARIABLE,
                                                 relink_[currentSource->data]));
            currentSource++;
            ArithmeticParser (currentNode, currentSource);
            break;
        }

        case TOKEN_NUM:
        {
            currentNode->SetElem (NodeContent_t (NODE_NUMBER, currentSource->data));
            currentSource++;
            ArithmeticParser (currentNode, currentSource);
            break;
        }

            default: break;
    }
    END (ARITHMETIC_PARSER)
}



void LolcodeLexicalAnalyzer_t::RecursiveAnalyzer (Node_t<NodeContent_t>* currentNode,
                                                  std::vector<Token_t>::iterator& currentSource,
                                                  Node_t<NodeContent_t>* currentFunction)
{

    BEGIN
    #define NOT_IN_A_FUNCTION \
    if (currentFunction == nullptr) \
    {\
        AddError (*old, "Not in a function"); \
        break;\
    }

    #define NEXT_NODE \
    currentNode = currentNode->GetParent (); \
    currentNode = currentNode->PushChild ();

    auto old = currentSource;

    switch (currentSource->type)
    {
        case TOKEN_I_HAS_A:
        {
            if (currentSource < (code_->tokens_.end () - 2) &&
                (currentSource + 2)->type == TOKEN_ITZ)
            {
                treeRoot_[0][relink_[(currentSource + 1)->data]].
                PushChild (NodeContent_t (NODE_NUMBER,
                                          (currentSource + 2)->data));
            }
            else
            {
                treeRoot_[0][relink_[currentSource->data]].
                PushChild (NodeContent_t (NODE_NUMBER, 0));
            }
            break;
        }

        case TOKEN_HOW_IZ_I:
        {
            if (currentFunction == nullptr)
            {
                currentSource += 2;
                treeRoot_[relink_[(currentSource - 1)->data]].PushChild ();
                RecursiveAnalyzer (&treeRoot_[relink_[(currentSource - 1)->data]][0],
                                   currentSource,
                                   &treeRoot_[relink_[(currentSource - 1)->data]]);
            }
            else
                AddError (*currentSource, "Nested functions are deprecated");
            break;
        }

        case TOKEN_IF_YOU_SAY_SO:
        {
            if (currentFunction != nullptr)
            {
                currentSource++;
                return;
            }
            else
                AddError (*currentSource, "Not in a function");
            break;
        }

        case TOKEN_OPENNG:
        {
            if (currentFunction == nullptr)
            {
                currentSource += 1;
                treeRoot_.GetLastChild ()->PushChild ();
                RecursiveAnalyzer (treeRoot_.GetLastChild ()->GetChild (0),
                                   currentSource,
                                   treeRoot_.GetLastChild ());
            }
            else
                AddError (*old, "Nested functions are deprecated");
            break;
        }

        case TOKEN_I_IZ:
        {
            NOT_IN_A_FUNCTION
            currentNode.SetElem (NodeContent_t (STD_FUNC_CALL_USER,
                                                relink_[(currentSource + 1)->data]);
            currentSource += 2;

            NEXT_NODE
            break;
        }

        case TOKEN_VAR:
        {
            NOT_IN_A_FUNCTION
            currentNode->SetElem (NodeContent_t (NODE_VARIABLE,
                                                 relink_[currentSource->data]));
            currentSource++;
            NEXT_NODE
            break;
        }

        case TOKEN_R:
        {
            NOT_IN_A_FUNCTION
            Node_t<NodeContent_t>* bak = currentNode;
            currentNode = currentNode->GetParent();
            uint32_t size = currentNode->GetNChildren ();

            if (currentNode->GetChild (size - 2)->GetElem ().flag != NODE_VARIABLE)
            {
                AddError (*old, "Invalid use of R");
            }
            else
            {
                currentNode->InsertAndSlide (size - 2,
                                             NodeContent_t (NODE_OPERATOR,
                                                            OP_EQUAL));
                currentSource++;
                ArithmeticParser (currentNode->PushChild (), currentSource);
            }
            currentNode = bak;
            break;
        }

        default: break;
    }

    if (currentSource == code_->tokens_.end ()) return;
    else RecursiveAnalyzer (currentNode,
                            (currentSource == old) ?
                            (++currentSource) :
                            currentSource,
                            currentFunction);


    #undef NOT_IN_A_FUNCTION

    END (RECURSIVE_ANALYZER)
}


    /**


    2. Start building tree
    On var find modify initialisation number node to match ITZ
    On function declaration iteratively run the tree building starting by function node
    On OPENNG add main node
    The RTB (recursive tree building function) checks the current node with a huge switch over all the possible token types, creating one or more needed subnodes starting from the current node
    var R PRODUKT var AN 6
    var -> creating node VAR current on child of VAR
    R -> creating node = current on 2 child of =
    PRODUKT -> creating node * current on child of *
    var -> creating node VAR, current on child of var
    AN -> deleting child of VAR, returning to parent, adding child, current on added child
    6 -> adding NUM, current on child of num
    Need some way to know the cmd has ended

    **/

