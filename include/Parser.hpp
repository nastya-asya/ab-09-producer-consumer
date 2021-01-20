// Copyright 2020 Anastasiya Smirnova nastya.asya08@yandex.ru

#ifndef INCLUDE_PARSER_HPP_
#define INCLUDE_PARSER_HPP_

#include <ThreadPool.h>
#include <gumbo.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>

#include "Parametrs.hpp"
#include "Queue.hpp"

class Parser {
 public:
  explicit Parser(unsigned poolsCount);
  void parsing();

 private:
  void writing();
  void search_for_links(GumboNode* g_node);
  ThreadPool pools;
  ThreadPool pools_output{1};
};

Parser::Parser(unsigned int poolsCount) : pools(poolsCount) {}
void Parser::parsing() {
  pools_output.enqueue(&Parser::writing, this);
  while (true) {
    if (queues_.empty()) {
      sleep(4);
      if (queues_.empty())
        break;
      else
        continue;
    }
    pools.enqueue(&Parser::search_for_links, this,
                  gumbo_parse(queues_.front().c_str())->root);
    queues_.pop();
  }
}

void Parser::writing() {
  std::ofstream fout;
  fout.open(parametrs.output);
  while (true) {
    if (pict_links.empty()) {
      sleep(4);
      if (pict_links.empty())
        break;
      else
        continue;
    }
    fout << pict_links.front() << std::endl;
    pict_links.pop();
  }
  fout.close();
}
void Parser::search_for_links(GumboNode* g_node) {
  if (g_node->type != GUMBO_NODE_ELEMENT) {
    return;
  }
  GumboAttribute* src = nullptr;
  if (g_node->v.element.tag == GUMBO_TAG_IMG &&
      (src = gumbo_get_attribute(&g_node->v.element.attributes, "src"))) {
    std::string str = src->value;
    std::cout << str << std::endl;
    pict_links.push(str);
  }
  GumboVector* children = &g_node->v.element.children;
  for (size_t i = 0; i < children->length; ++i) {
    search_for_links(static_cast<GumboNode*>(children->data[i]));
  }
}

#endif  // INCLUDE_PARSER_HPP_
