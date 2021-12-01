#include <btb/graphql.hpp>
#include <eosio/reflection2.hpp>
#include "example.hpp"

// GraphQL proxy for example::animal
struct Animal
{
   const example::animal* obj;

   // These methods have no arguments, so act like fields in GraphQL
   auto name() const { return obj->name; }
   auto type() const { return obj->type; }
   auto owner() const { return obj->owner; }
   auto purchasePrice() const { return obj->purchase_price; }

   // This method has an argument, so needs method(...) in the
   // EOSIO_REFLECT2 definition below.
   auto isA(eosio::name type) const { return type == obj->type; }
};
EOSIO_REFLECT2(Animal, name, type, owner, purchasePrice, method(isA, "type"))

struct Query
{
   eosio::name contract;

   // Returns a Proxy object instead of returning the original object
   std::optional<Animal> animal(eosio::name name) const
   {
      example::animal_table table{contract, contract.value};
      auto it = table.find(name.value);
      if (it != table.end())
         return Animal{&*it};
      else
         return std::nullopt;
   }
};
EOSIO_REFLECT2(Query,                  //
               contract,               // query a field
               method(animal, "name")  // query a method; identifies the argument names
)

void example::example_contract::graphql(const std::string& query)
{
   Query root{get_self()};
   eosio::print(btb::gql_query(root, query, ""));
}

void example::example_contract::graphqlschema()
{
   eosio::print(btb::get_gql_schema<Query>());
}
