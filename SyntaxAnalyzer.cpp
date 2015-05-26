
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

    void Dump (std::string filename);
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

    treeRoot_.GetLastChild ()->ClearLastChild ();

    END (PROCESS)
}

void LolcodeLexicalAnalyzer_t::ArithmeticParser (Node_t<NodeContent_t>* currentNode,
                                                 std::vector<Token_t>::iterator& currentSource)
{
    BEGIN
    #define CASE_END \
        currentSource++; \
        ArithmeticParser (currentNode->PushChild (), currentSource); \
        ArithmeticParser (currentNode->GetLastChild (), currentSource); \
        if (currentNode->GetNChildren () != 2) AddError (*old, "Invalid amount ov arguments");

    auto old = currentSource;
    //printf ("%d %g\n", currentSource->type, currentSource->data);
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
        case TOKEN_SQRT_OF:
            currentNode->SetElem (NodeContent_t (NODE_STD_FUNCTION, STD_FUNC_SQRT));
            currentSource++;
            //printf ("Sqrt, about to rec \n");
            ArithmeticParser (currentNode->PushChild (), currentSource);
            //ArithmeticParser (currentNode->GetLastChild (), currentSource);
            if (currentNode->GetNChildren () != 1) AddError (*old, "Invalid amount ov arguments");
            break;
        case TOKEN_SUM_OF:
            currentNode->SetElem (NodeContent_t (NODE_OPERATOR, OP_PLUS));
            CASE_END
            break;
        case TOKEN_PWR_OF:
            currentNode->SetElem (NodeContent_t (NODE_OPERATOR, OP_POWER));
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
            currentNode->SetElem (NodeContent_t (NODE_STD_FUNCTION, STD_FUNC_MIN));
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
            (*currentNode)[0].PushChild (NodeContent_t (NODE_OPERATOR, OP_BOOL_NOT));
            (*currentNode)[0][1].PushChild ();

            currentNode->InsertAndSlide (1, NodeContent_t (NODE_OPERATOR, OP_BOOL_AND));
            (*currentNode)[1].PushChild (NodeContent_t (NODE_OPERATOR, OP_BOOL_NOT));
            (*currentNode)[1][1].PushChild ();

            CopyTree (&(*currentNode)[0][0], &(*currentNode)[1][1][0]);
            CopyTree (&(*currentNode)[1][0], &(*currentNode)[0][1][0]);

            break;

        case TOKEN_AN:
            currentNode = currentNode->GetParent ();
            currentSource++;
            ArithmeticParser (currentNode->PushChild (), currentSource);
            break;

        case TOKEN_NOT:
            currentNode->SetElem (NodeContent_t (NODE_OPERATOR, OP_BOOL_NOT));
            currentSource++;
            ArithmeticParser (currentNode->PushChild (), currentSource);
            break;

        case TOKEN_COS_OF:
            currentNode->SetElem (NodeContent_t (NODE_STD_FUNCTION, STD_FUNC_COS));
            currentSource++;
            ArithmeticParser (currentNode->PushChild (), currentSource);
            if (currentNode->GetNChildren () != 1) AddError (*old, "Invalid amount ov arguments");
            break;

        case TOKEN_SIN_OF:
            currentNode->SetElem (NodeContent_t (NODE_STD_FUNCTION, STD_FUNC_SIN));
            currentSource++;
            ArithmeticParser (currentNode->PushChild (), currentSource);
            if (currentNode->GetNChildren () != 1) AddError (*old, "Invalid amount ov arguments");
            break;

        case TOKEN_DER_OF:
            currentNode->SetElem (NodeContent_t (NODE_STD_FUNCTION, STD_FUNC_DIFFERENTIATE));
            CASE_END
            break;

        case TOKEN_VAR:
        {
            currentNode->SetElem (NodeContent_t (NODE_VARIABLE,
                                                 relink_[currentSource->data]));
            currentSource++;
            //ArithmeticParser (currentNode, currentSource);
            break;
        }

        case TOKEN_NUM:
        {
            currentNode->SetElem (NodeContent_t (NODE_NUMBER, currentSource->data));
            currentSource++;
            //ArithmeticParser (currentNode, currentSource);
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
            if (currentSource < (code_->tokens_.end () - 3) &&
                (currentSource + 2)->type == TOKEN_ITZ)
            {
                treeRoot_[0][relink_[(currentSource + 1)->data]].
                PushChild (NodeContent_t (NODE_NUMBER,
                                          (currentSource + 3)->data));
                currentSource += 4;
            }
            else
            {
                currentSource += 1;
                treeRoot_[0][relink_[currentSource->data]].
                PushChild (NodeContent_t (NODE_NUMBER, 0));
                currentSource += 1;
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
                currentNode->GetParent ()->ClearLastChild ();
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
            currentSource += 1;
            currentNode->SetElem (NodeContent_t (NODE_STD_FUNCTION, STD_FUNC_CALL_USER));
            currentNode->PushChild (NodeContent_t (NODE_USER_FUNCTION, relink_[currentSource->data]));
            currentSource += 1;
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
                ArithmeticParser (currentNode->GetChild (size - 2)->PushChild (), currentSource);
            }
            currentNode = bak;
            break;
        }

        case TOKEN_IM_IN_YR:
        {
            NOT_IN_A_FUNCTION
            Node_t<NodeContent_t>* bak = currentNode->GetParent ();
            const int UPPIN = 1;
            const int NERFIN = 2;
            const int NOTHIN = 0;
            char up_nerf = NOTHIN;

            if ((currentSource >= code_->tokens_.end () - 2) ||
                ((currentSource + 1)->type != TOKEN_LOOP &&
                (currentSource + 2)->type != TOKEN_UPPIN_YR &&
                (currentSource + 2)->type != TOKEN_NERFIN_YR &&
                (currentSource + 2)->type != TOKEN_TIL &&
                (currentSource + 2)->type != TOKEN_WILE))
            {
                AddError (*currentSource, "Invalid loop usage");
            }

            currentSource += 2;

            currentNode->SetElem (NodeContent_t (NODE_LOGIC, LOGIC_WHILE));

            if (currentSource->type == TOKEN_UPPIN_YR) up_nerf = UPPIN;
            else
            if (currentSource->type == TOKEN_NERFIN_YR) up_nerf = NERFIN;

            if (up_nerf != NOTHIN)
            {

                currentSource++;
                uint32_t var = relink_[currentSource->data];
                currentNode->PushChild (NodeContent_t (NODE_LOGIC, LOGIC_CONDITION));
                currentSource++;
                if (currentSource->type == TOKEN_TIL)
                    ArithmeticParser ((*currentNode)[0].
                                      PushChild (NodeContent_t (OP_BOOL_NOT))->
                                      PushChild (),
                                      (++currentSource));
                else
                    ArithmeticParser ((*currentNode)[0].PushChild (), (++currentSource));

                currentNode->PushChild (NodeContent_t (NODE_LOGIC, LOGIC_CONDITION_MET));
                RecursiveAnalyzer ((*currentNode)[1].PushChild (), currentSource, currentFunction);
                (*currentNode)[1].PushChild (NodeContent_t (NODE_OPERATOR, OP_EQUAL));
                (*currentNode)[1].GetLastChild ()->PushChild (NodeContent_t (NODE_VARIABLE, var));
                if (up_nerf == UPPIN)
                    (*currentNode)[1].GetLastChild ()->PushChild (NodeContent_t (NODE_OPERATOR, OP_PLUS));
                else
                    (*currentNode)[1].GetLastChild ()->PushChild (NodeContent_t (NODE_OPERATOR, OP_MINUS));

                (*currentNode)[1].GetLastChild ()->GetLastChild ()->PushChild (NodeContent_t (NODE_VARIABLE, var));
                (*currentNode)[1].GetLastChild ()->GetLastChild ()->PushChild (NodeContent_t (NODE_NUMBER, 1));

            }
            else
            {

                currentNode->PushChild (NodeContent_t (NODE_LOGIC, LOGIC_CONDITION));
                if (currentSource->type == TOKEN_TIL)
                    ArithmeticParser ((*currentNode)[0].
                                      PushChild (NodeContent_t (OP_BOOL_NOT))->
                                      PushChild (),
                                      (++currentSource));
                else
                    ArithmeticParser ((*currentNode)[0].PushChild (), (++currentSource));

                currentNode->PushChild (NodeContent_t (NODE_LOGIC, LOGIC_CONDITION_MET));
                RecursiveAnalyzer ((*currentNode)[1].PushChild (), currentSource, currentFunction);

            }

            currentNode = bak->PushChild ();
            break;
        }

        case TOKEN_IM_OUTTA_YR:
        {
            NOT_IN_A_FUNCTION

            if ((currentSource == code_->tokens_.end () - 1) ||
                (currentSource + 1)->type != TOKEN_LOOP)
            {
                AddError (*currentSource, "Invalid loop exit usage, expected loop name");
            }
            currentSource += 2;
            currentNode->GetParent ()->ClearLastChild ();
            return;
        }

        case TOKEN_NO_WAI:
        {
            NOT_IN_A_FUNCTION
            currentSource++;
            currentNode->GetParent ()->ClearLastChild ();
            return;
        }

        case TOKEN_OIC:
        {
            NOT_IN_A_FUNCTION
            currentSource++;
            currentNode->GetParent ()->ClearLastChild ();
            return;
        }

        case TOKEN_O_RLY_QM:
        {
            NOT_IN_A_FUNCTION

            Node_t<NodeContent_t>* bak = currentNode;

            if ((currentSource == code_->tokens_.end () - 1) ||
                ((currentSource + 1)->type != TOKEN_YA_RLY))
            {
                AddError (*currentSource, "Invalid if usage, expected \"YA RLY\"");
            }

            currentSource += 2;

            currentNode = currentNode->GetParent ();

            currentNode = currentNode->InsertAndSlide (currentNode->GetNChildren () - 2,
                                                       NodeContent_t (NODE_LOGIC, LOGIC_IF));

            currentNode->InsertAndSlide (0, NodeContent_t (NODE_LOGIC, LOGIC_CONDITION));

            currentNode->PushChild (NodeContent_t (NODE_LOGIC, LOGIC_CONDITION_MET));
            RecursiveAnalyzer ((*currentNode)[1].PushChild (), currentSource, currentFunction);
            if ((currentSource - 1)->type == TOKEN_NO_WAI)
            {
                currentNode->PushChild (NodeContent_t (NODE_LOGIC, LOGIC_ELSE));
                RecursiveAnalyzer ((*currentNode)[2].PushChild (), currentSource, currentFunction);
            }
            if ((currentSource - 1)->type != TOKEN_OIC)
                AddError (*currentSource, "Invalid if ending, expected \"OIC\"");

            currentNode = bak;
            break;
        }

        case TOKEN_BOTH_SAEM:
        case TOKEN_DIFFRINT:
        case TOKEN_SUM_OF:
        case TOKEN_DIFF_OF:
        case TOKEN_PRODUKT_OF:
        case TOKEN_QUOSHUNT_OF:
        case TOKEN_BIGGR_OF:
        case TOKEN_SMALLR_OF:
        case TOKEN_EITHER_OF:
        case TOKEN_BOTH_OF:
        case TOKEN_WON_OF:
        case TOKEN_PWR_OF:
        case TOKEN_AN:
        case TOKEN_NOT:
        case TOKEN_COS_OF:
        case TOKEN_SIN_OF:
        case TOKEN_SQRT_OF:
        case TOKEN_DER_OF:
        {
            auto bak = currentNode->GetParent();
            ArithmeticParser (currentNode, currentSource);
            currentNode = bak->PushChild ();
            break;
        }

        case TOKEN_VISIBLE:
        {
            currentNode->SetElem   (NodeContent_t (NODE_STD_FUNCTION, STD_FUNC_OUTPUT));
            currentSource++;
            ArithmeticParser (currentNode->PushChild (), currentSource);

            NEXT_NODE
            break;
        }

        case TOKEN_GIMMEH:
        {
            currentNode->SetElem   (NodeContent_t (NODE_STD_FUNCTION, STD_FUNC_INPUT));
            currentSource++;
            ArithmeticParser (currentNode->PushChild (), currentSource);
            NEXT_NODE
            break;
        }


        case TOKEN_GETCH:
        {
            currentNode->SetElem   (NodeContent_t (NODE_STD_FUNCTION, STD_FUNC_GETCH));
            currentSource++;
            NEXT_NODE
            break;
        }

        default: _EXC_N (UNKNOWN_TOKEN, "Found unknown token"); break;
    }


    if (currentSource == code_->tokens_.end ())
        return;
    else RecursiveAnalyzer (currentNode,
                            (currentSource == old) ?
                            (++currentSource) :
                            currentSource,
                            currentFunction);


    #undef NOT_IN_A_FUNCTION

    END (RECURSIVE_ANALYZER)
}



void LolcodeLexicalAnalyzer_t::Dump (std::string filename)
{
    BEGIN
    File_t file (filename, "w");
    fprintf (*file, "%d %d\n", BUILD_NUMBER, PERSONAL_CODE);

    fprintf (*file, "FUNCS %d\n", funcs_.size () - 2);
    for (auto i = funcs_.data.begin () + 1;
         i < funcs_.data.end () - 1;
         i++)
        fprintf (*file, "%s\n", i->c_str ());

    fprintf (*file, "VARS %d\n", vars_.size ());
    for (auto i = vars_.data.begin ();
         i < vars_.data.end ();
         i++)
        fprintf (*file, "%s\n", i->c_str ());

    treeRoot_.DumpPrefix (file);
    END (DUMP)
}











