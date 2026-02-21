# Data storage system

* The data consists of a set of objects-with-properties.
* Each object is identified by a GUID.
* Each property is identified by a string.
* The property value can be null, a bool, a double, a vector3, a quaternion, a string, a data blob, a GUID or a set of GUIDs.
* The data has a root object with GUID 0.

```cs
class Database
{
    Dictionary<Guid, Object> db;
    /// creates the object with the specified GUID
    void create(Guid guid)
    {
        // ...
    }
    
    /// destroys the object with the specified GUID
    void destroy(Guid guid)
    {
        // ...
    }
    /// sets the specified property of the object to the value (set to nil to remove the property)
    void set_property(Guid guid, string key, value)
    {
        // ...
    }

    /// adds the item to the GUID set property identified by the key
    void add_to_set(Guid guid, string key, Guid item_guid)
    {
        // ...
    }
    
    /// removes the item from the GUID set property identified by the key 
    void remove_from_set(Guid guid, String key, Guid item_guid)
    {
        // ...
    }
}

class Object
{
    Dictionary<string, Value> values;
}

class Value
{
    // null
    // bool
    // double
    // vector3
    // quaternion
    // string
    // data blob
    // GUID
    // set of GUIDs
    // array of GUIDs?
}
```

### Typesafe interface
```cs
class SoundInstance
{
    Database db;
    Guid Guid;

    public SoundInstance(Database db)
    {
        this.db = db;
        Guid = Guid.NewGuid().ToString();
        db.create(Guid);
    }

    public String File
    {
        get { ... }
        set { db.set_property( Guid, "File", value ); }
    }
}
```

### Use meta language to generate typesafe interface

Does the meta langauge need to be a custom language or can we simply just define the data in code and codegen the typesafe interfaces?

```kdl
package data

struct Path
{
    array Point points
}

struct Point
{
    prop vec3 in
    prop vec3 point
    prop vec3 out
}
```

### References
https://bitsquid.blogspot.com/2010/08/new-data-storage-model.html
https://bitsquid.blogspot.com/2011/03/collaboration-and-merging.html
https://bitsquid.blogspot.com/2011/04/universal-undo-copy-and-paste.html