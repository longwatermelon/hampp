# hampp

ham written in cpp

If you want to build the project yourself, download simple-json-lib from [here](https://github.com/longwatermelon/simple-json-lib) and then use

```
g++ -I src -I (directory jsonlib.h is in) src/*.cpp -o build/hampp -std=c++17
```

# massive bug that i refuse to fix
if any struct has a member name the same as a struct member previously defined, hampp will find the first occurrence of the name when referencing that variable.

example

```
strooct foo
{
  def name = "foo";
};

strooct bar
{
  def name = "bar";
};

def foo = new foo;
def bar = new bar;

prount(foo.name, bar.name);
```

prints `foo foo`

probably will not be fixed because i have to do a massive refactor to fix this
