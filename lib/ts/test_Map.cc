/** @file

  Test code for the Map templates.

  @section license License

  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/
#include <stdint.h>
#include "Map.h"

typedef const char cchar;

struct Item {
  LINK(Item, m_link);
  struct Hash {
    typedef uint32_t ID;
    typedef uint32_t Key;
    typedef Item Value;
    typedef DList(Item, m_link) ListHead;

    static ID
    hash(Key key)
    {
      return key;
    }
    static Key key(Value *);
    static bool equal(Key lhs, Key rhs);
  };

  uint32_t _key;
  uint32_t _value;

  Item(uint32_t x) : _key(x), _value(x) {}
  Item(uint32_t key, uint32_t value) : _key(key), _value(value) {}
};

uint32_t
Item::Hash::key(Value *v)
{
  return v->_key;
}
bool
Item::Hash::equal(Key lhs, Key rhs)
{
  return lhs == rhs;
}

typedef TSHashTable<Item::Hash> Table;

void
test_TSHashTable()
{
  static uint32_t const N = 270;
  Table t;
  Item *item;
  Table::Location loc;

  item = new Item(1);
  t.insert(item);
  for (uint32_t i = 2; i <= N; ++i) {
    t.insert(new Item(i));
  }

  for (uint32_t i = 1; i <= N; ++i) {
    Table::Location l = t.find(i);
    ink_assert(l.isValid());
    ink_assert(i == l->_value);
  }

  ink_assert(!(t.find(N * 2).isValid()));

  loc = t.find(N / 2 | 1);
  if (loc)
    t.remove(loc);
  else
    ink_assert(!"Did not find expected value");

  if (!loc)
    ; // compiler check.

  ink_assert(!(t.find(N / 2 | 1).isValid()));

  for (uint32_t i = 1; i <= N; i += 2) {
    t.remove(i);
  }

  for (uint32_t i = 1; i <= N; ++i) {
    Table::Location l = t.find(i);
    if (1 & i)
      ink_assert(!l.isValid());
    else
      ink_assert(l.isValid());
  }

  int n = 0;
  for (Table::iterator spot = t.begin(), limit = t.end(); spot != limit; ++spot) {
    ++n;
    ink_assert((spot->_value & 1) == 0);
  }
  ink_assert(n == N / 2);
}

int
main(int /* argc ATS_UNUSED */, char ** /*argv ATS_UNUSED */)
{
  typedef Map<cchar *, cchar *> SSMap;
  typedef MapElem<cchar *, cchar *> SSMapElem;
#define form_SSMap(_p, _v) form_Map(SSMapElem, _p, _v)
  SSMap ssm;
  ssm.put("a", "A");
  ssm.put("b", "B");
  ssm.put("c", "C");
  ssm.put("d", "D");
  form_SSMap(x, ssm) { /* nop */}

  /*
    if ((ssm).n)
      for (SSMapElem *qq__x = (SSMapElem*)0, *x = &(ssm).v[0];
               ((intptr_t)(qq__x) < (ssm).n) && ((x = &(ssm).v[(intptr_t)qq__x]) || 1);
               qq__x = (SSMapElem*)(((intptr_t)qq__x) + 1))
            if ((x)->key) {
              // nop
            }
            */

  StringChainHash<> h;
  cchar *hi = "hi", *ho = "ho", *hum = "hum", *hhi = "hhi";
  hhi++;
  h.put(hi);
  h.put(ho);
  ink_assert(h.put(hum) == hum);
  ink_assert(h.put(hhi) == hi);
  ink_assert(h.get(hhi) == hi && h.get(hi) == hi && h.get(ho) == ho);
  ink_assert(h.get("he") == 0 && h.get("hee") == 0);
  h.del(ho);
  ink_assert(h.get(ho) == 0);

  StringBlockHash hh;
  hh.put(hi);
  hh.put(ho);
  ink_assert(hh.put(hum) == 0);
  ink_assert(hh.put(hhi) == hi);
  ink_assert(hh.get(hhi) == hi && hh.get(hi) == hi && hh.get(ho) == ho);
  ink_assert(hh.get("he") == 0 && hh.get("hee") == 0);
  hh.del(hi);
  ink_assert(hh.get(hhi) == 0);
  ink_assert(hh.get(hi) == 0);

  HashMap<cchar *, StringHashFns, int> sh;
  sh.put(hi, 1);
  sh.put(ho, 2);
  sh.put(hum, 3);
  sh.put(hhi, 4);
  ink_assert(sh.get(hi) == 4);
  ink_assert(sh.get(ho) == 2);
  ink_assert(sh.get(hum) == 3);
  sh.put("aa", 5);
  sh.put("ab", 6);
  sh.put("ac", 7);
  sh.put("ad", 8);
  sh.put("ae", 9);
  sh.put("af", 10);
  ink_assert(sh.get(hi) == 4);
  ink_assert(sh.get(ho) == 2);
  ink_assert(sh.get(hum) == 3);
  ink_assert(sh.get("af") == 10);
  ink_assert(sh.get("ac") == 7);

  ChainHashMap<cchar *, StringHashFns, int> ssh;
  ssh.put(hi, 1);
  ssh.put(ho, 2);
  ssh.put(hum, 3);
  ssh.put(hhi, 4);
  ink_assert(ssh.get(hi) == 4);
  ink_assert(ssh.get(ho) == 2);
  ink_assert(ssh.get(hum) == 3);
  ssh.put("aa", 5);
  ssh.put("ab", 6);
  ssh.put("ac", 7);
  ssh.put("ad", 8);
  ssh.put("ae", 9);
  ssh.put("af", 10);
  ink_assert(ssh.get(hi) == 4);
  ink_assert(ssh.get(ho) == 2);
  ink_assert(ssh.get(hum) == 3);
  ink_assert(ssh.get("af") == 10);
  ink_assert(ssh.get("ac") == 7);
  ssh.del(ho);
  ink_assert(ssh.get(ho) == 0);

  Vec<int> ints;
  ssh.get_values(ints);
  ink_assert(ints.n == 8);
  Vec<cchar *> chars;
  ssh.get_keys(chars);
  ink_assert(chars.n == 8);


  test_TSHashTable();

  printf("test_Map PASSED\n");
}
