# hampp

ham written in cpp

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
