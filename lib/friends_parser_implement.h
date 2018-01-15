/**
 * @file friends_parser_implement.h
 */
/* -*- mode: c -*- */
/* vim: set ft=c: */

#ifndef FRIENDS_PARSER_IMPLEMENT_H
#define FRIENDS_PARSER_IMPLEMENT_H

#include "friends_defs.h"

friendsData *
friendsParserAddDataToTerm(friendsData *term, friendsData *data,
                           friendsPark *park, friendsError *err);

friendsData *
friendsParserCreateAtom(friendsPark *park, friendsAtomType type,
                        friendsToken *token_data, friendsError *err);

friendsData *
friendsParserCreateVariable(friendsPark *park, friendsToken *token_data,
                            friendsBool tail, friendsError *err);

friendsData *
friendsParserCreateProposition(friendsPark *park,
                               friendsDataList *conds, friendsDataList *args,
                               friendsToken *verb_token,
                               friendsPropositionMode mode,
                               friendsError *err);

void friendsParserInsertProposition(friendsDataList *parsed_data,
                                    friendsPark *park,
                                    friendsDataList *conds,
                                    friendsDataList *args,
                                    friendsToken *verb_data,
                                    friendsPropositionMode mode,
                                    friendsError *err);

friendsData *friendsParserCreateListTerm(friendsPark *park,
                                         friendsDataList *list,
                                         friendsData *add_data,
                                         friendsError *err);

void friendsPrintSyntaxError(friendsParser *parser, friendsToken *token,
                             int yytoken, const char *yyTokenName);

#endif /* FRIENDS_PARSER_IMPLEMENT_H */
