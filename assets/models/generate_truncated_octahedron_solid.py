#----------------------------------------------------------
# File meshes.py
#----------------------------------------------------------
import bpy

def createMesh(name, origin, verts, edges, faces):
    # Create mesh and object
    me = bpy.data.meshes.new(name+'Mesh')
    ob = bpy.data.objects.new(name, me)
    ob.location = origin
    ob.show_name = True
    # Link object to scene
    bpy.context.collection.objects.link(ob)

    # Create mesh from given verts, edges, faces. Either edges or
    # faces should be [], or you ask for problems
    me.from_pydata(verts, edges, faces)

    # Update mesh with new data
    me.update(calc_edges=True)
    return ob

def run(origin):
    c0 = 0.5
    c1 = 1

    verts1 = (( c0, 0.0, c1),
              (c0, 0.0, -c1),
              (-c0, 0.0, c1),
              (-c0, 0.0, -c1),
              ( c1, c0, 0.0),
              ( c1, -c0, 0.0),
              (-c1,  c0, 0.0),
              (-c1, -c0, 0.0),
              (0.0, c1, c0),
              (0.0, c1, -c0),
              (0.0, -c1, c0),
              (0.0, -c1, -c0),
              (0.0, c0,  c1),
              (0.0, c0, -c1),
              (0.0, -c0,  c1),
              (0.0, -c0, -c1),
              ( c1, 0.0,  c0),
              ( c1, 0.0, -c0),
              (-c1, 0.0,  c0),
              (-c1, 0.0, -c0),
              (c0,  c1, 0.0),
              ( c0, -c1, 0.0),
              (-c0,  c1, 0.0),
              (-c0, -c1, 0.0))

    faces1 = ((0, 14, 10, 21,  5, 16),
              (1, 13,  9, 20,  4, 17),
              (2, 12,  8, 22,  6, 18),
              (3, 15, 11, 23,  7, 19),
              (4, 20,  8, 12,  0, 16),
              (5, 21, 11, 15,  1, 17),
              (7, 23, 10, 14,  2, 18),
              (6, 22,  9, 13,  3, 19),
              (0, 12,  2, 14),
              (1, 15,  3, 13),
              (4, 16,  5, 17),
              (6, 19,  7, 18),
              (8, 20,  9, 22),
              (10, 23, 11, 21))

    ob1 = createMesh('Solid', origin, verts1, [], faces1)

    return

if __name__ == "__main__":
    run((0,0,0))