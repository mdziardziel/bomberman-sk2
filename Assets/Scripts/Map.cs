using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Map : MonoBehaviour{

    protected int mapSizeX, mapSizeY;
    public enum thingInPlace { empty, wall, box };
    protected thingInPlace[,] space;
    protected Hashtable softWalls;


    public void  Initialize(int x, int y) {
        softWalls = new Hashtable();
        mapSizeX = x - 1;
        mapSizeY = y - 1;
        space = new thingInPlace[x, y];
        for (int i = 0; i < mapSizeX; i++)
            for (int j = 0; j < mapSizeY; j++)
                if (i % 2 == 1 && j % 2 == 1) {
                    space[i, j] = thingInPlace.wall;
                  
                }
    }

    public void AddBox(int x, int y,GameObject obj) {
        space[x, y] = thingInPlace.box;
        softWalls.Add(x.ToString() + y.ToString(),Instantiate(obj,new Vector3(x,0,y),Quaternion.identity));
    }

    public thingInPlace getObjectInPoint(int x, int y) {
        return space[x,y];
    }

    public bool checkIfWalkable(int x, int y) {
        if (space[x, y] == thingInPlace.empty)
            return true;
        else
            return false;
    }
    
    public int getSize(char dir) {
        if (dir == 'x')
            return mapSizeX;
        else
            return mapSizeY;
    }

    public bool destroyIfSoft(int x, int y) {
        if(space[x,y] == thingInPlace.box) {
            space[x, y] = thingInPlace.empty;
            GameObject temp = softWalls[x.ToString() + y.ToString()] as GameObject;          
            softWalls.Remove(x.ToString() + y.ToString());
            Destroy(temp);
            return true;
        } else {
            return false;
        }
    }
   
}
