using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : MonoBehaviour {
    protected MeshRenderer mesh;
    protected string playerName;
    protected bool readyStatus;
    protected int playerPoints;
    protected Vector2Int position;
    protected Vector3 lastPosition;
    protected Vector3 newPosition;
    protected float movingTime;
    protected bool isDead;
    protected float deadTimer;
    protected bool mainPlayer;
    protected bool empty = true;
    protected bool ableToPlay;

    public string getName() {
        return playerName;
    }

    public bool isReady() {
        return readyStatus;
    }

    public bool amiDead() {
        return isDead;
    }

    public bool isPositioned() {
        return ableToPlay;
    }

    public bool isEmpty() {
        return empty;
    }

    public int getPoints() {
        return playerPoints;
    }

    public bool isMain() {
        return mainPlayer;
    }



    public Player() {
        ClearObject();
    }

    public void assignMesh() {
        MeshFilter[] meshFilters = GetComponentsInChildren<MeshFilter>();
        CombineInstance[] combine = new CombineInstance[meshFilters.Length];
        gameObject.transform.position = Vector3.zero;
        for (int i = 0; i < meshFilters.Length; i++) {
            combine[i].mesh = meshFilters[i].sharedMesh;
            combine[i].transform = meshFilters[i].transform.localToWorldMatrix;
            meshFilters[i].gameObject.SetActive(false);
        }
        transform.GetComponent<MeshFilter>().mesh = new Mesh();
        transform.GetComponent<MeshFilter>().mesh.CombineMeshes(combine);
        mesh = gameObject.GetComponent<MeshRenderer>();
        mesh.enabled = false;
        gameObject.transform.position = new Vector3(-1, -1, -1);
        gameObject.SetActive(true);
    }


    public void setName(string _name) {
        playerName = _name;
        empty = false;
    }

    public void setPoints(int _points) {
        playerPoints = _points;
    }

    public void setReady(bool ready) {
        readyStatus = ready;
    }

    public void setDead(bool dead) {
        isDead = dead;
        if(dead)
            deadTimer = 5.0f;
    }

    public void setMain(bool main = true) {
        mainPlayer = true;
        gameObject.tag = "Player";
    }

    public void setPosition(int x,int y) {
        position = new Vector2Int(x, y);
        gameObject.transform.position = new Vector3(x, 0, y);
        lastPosition = gameObject.transform.position;
        newPosition = gameObject.transform.position;
        ableToPlay = true;
    }

    public void movePlayer(int x, int y) {
        if(position.x != x || position.y != y) {
            lastPosition = new Vector3(position.x, 0, position.y);
            newPosition = new Vector3(x, 0, y);
            movingTime = 0.0f;
            position = new Vector2Int(x, y);
        }
    }

    public void InstantiateObject() {
        if (mainPlayer) {
            gameObject.AddComponent<PlayerController>();
        }
    }

    public void ClearObject() {
        empty = true;
        playerName = "";
        playerPoints = 0;
        mainPlayer = false;
        isDead = true;
        movingTime = 0.5f;
        position = new Vector2Int(-1, -1);
        deadTimer = 0.0f;
        ableToPlay = false;
    }

    public void BackToLobby() {
        playerPoints = 0;
        isDead = true;
        movingTime = 0.0f;
        position = new Vector2Int(-1, -1);
        gameObject.transform.position = new Vector3(-1, -1, -1);
        
        deadTimer = 0.0f;
        ableToPlay = false;
        if (mainPlayer)
            Destroy(gameObject.GetComponent<PlayerController>());
    }
   

    // Update is called once per frame
    void LateUpdate()
    {
        if (ableToPlay) {
            if (!mainPlayer) {
                if (isDead) {
                    mesh.enabled = false;
                } else {
                    mesh.enabled = true;
                }
            } else {
                mesh.enabled = true;
                if (isDead) {
                    mesh.material.color = new Color(0.2f,0.2f,1,0.2f);
                } else {
                    mesh.material.color = new Color(0.1f, 1, 0.1f, 1);
                }
            }


            if (deadTimer >= 0.0f) {
                deadTimer -= Time.deltaTime;
            } else if (isDead) {
                setDead(false);
            }


            if (movingTime <= 0.25f) {
                float percent = movingTime / 0.25f;
                gameObject.transform.position = Vector3.Lerp(lastPosition, newPosition, percent);
                movingTime += Time.deltaTime;
            }
        }
    }
}
