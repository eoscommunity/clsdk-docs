#include <eosio/tester.hpp>
#include <token/token.hpp>  // comes bundled with clsdk
#include "testable.hpp"

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

using namespace eosio;

// Set up the token contract
void setup_token(test_chain& t)
{
   t.create_code_account("eosio.token"_n);
   t.set_code("eosio.token"_n, CLSDK_CONTRACTS_DIR "token.wasm");

   // Create and issue tokens.
   t.as("eosio.token"_n).act<token::actions::create>("eosio"_n, s2a("1000000.0000 EOS"));
   t.as("eosio.token"_n).act<token::actions::create>("eosio"_n, s2a("1000000.0000 OTHER"));
   t.as("eosio"_n).act<token::actions::issue>("eosio"_n, s2a("1000000.0000 EOS"), "");
   t.as("eosio"_n).act<token::actions::issue>("eosio"_n, s2a("1000000.0000 OTHER"), "");
}

// Create and fund user accounts
void fund_users(test_chain& t)
{
   for (auto user : {"alice"_n, "bob"_n, "jane"_n, "joe"_n})
   {
      t.create_account(user);
      t.as("eosio"_n).act<token::actions::transfer>("eosio"_n, user, s2a("10000.0000 EOS"), "");
      t.as("eosio"_n).act<token::actions::transfer>("eosio"_n, user, s2a("10000.0000 OTHER"), "");
   }
}

// Set up the example contract
void setup_example(test_chain& t)
{
   t.create_code_account("example"_n);
   t.set_code("example"_n, "testable.wasm");
}

// Full setup for test chain
void setup(test_chain& t)
{
   setup_token(t);
   fund_users(t);
   setup_example(t);
}

//////

template <typename Table>
void dump_table(name contract, uint64_t scope)
{
   Table table(contract, scope);
   for (auto& record : table)
      std::cout << format_json(record) << "\n";
}

TEST_CASE("Read Database 2")
{
   test_chain chain;
   setup(chain);

   chain.as("alice"_n).act<token::actions::transfer>(
       "alice"_n, "example"_n, s2a("300.0000 EOS"), "");
   chain.as("alice"_n).act<example::actions::buydog>(
       "alice"_n, "fido"_n, s2a("100.0000 EOS"));
   chain.as("alice"_n).act<example::actions::buydog>(
       "alice"_n, "barf"_n, s2a("110.0000 EOS"));
   chain.as("bob"_n).act<token::actions::transfer>(
       "bob"_n, "example"_n, s2a("300.0000 EOS"), "");
   chain.as("bob"_n).act<example::actions::buydog>(
       "bob"_n, "wolf"_n, s2a("100.0000 EOS"));

   printf("\nBalances\n=====\n");
   dump_table<example::balance_table>("example"_n, "example"_n.value);

   printf("\nAnimals\n=====\n");
   dump_table<example::animal_table>("example"_n, "example"_n.value);
}
