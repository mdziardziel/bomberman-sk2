using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class GlobalController : MonoBehaviour {

    public static GlobalController Instance;
    public GameObject player;
    public GameObject bombObj;
    public int myNumber = -1;
    public String MyName;
    public GameObject[] playersGameObjects;
    public Player[] playerList;
    public Map gameMap;
    public bool ended;

    public bool playersUpdated;

    private void Awake() {
        if (Instance == null) {
            DontDestroyOnLoad(gameObject);
            

            Instance = this;


            playersGameObjects = new GameObject[8];
            playerList = new Player[8];

            for (int i = 0; i < 8; i++) {
                playersGameObjects[i] = Instantiate(player, new Vector3(-1, -1, -1), Quaternion.identity);
                playersGameObjects[i].AddComponent<Player>();
                DontDestroyOnLoad(playersGameObjects[i]);
                playerList[i] = playersGameObjects[i].GetComponent<Player>();
                playerList[i].assignMesh();
                playerList[i].ClearObject();


            }

        } else if (Instance != this) {
            Destroy(gameObject);
        }

    }

    public void PlaceBombInPoint(int pla, int x, int y) {
        GameObject boomb = Instantiate(bombObj, new Vector3(x, -0.1f, y), Quaternion.identity);
        boomb.GetComponent<DestroyBomb>().owningPlayer = pla;
    }


    public void UpdatePlayers() {
        for (int i = 0; i < 7; i++) {
            if (playerList[i].isEmpty()) {
                GameObject temp = playersGameObjects[i];
                playersGameObjects[i] = playersGameObjects[i + 1];
                Destroy(temp);
                playerList[i] = playersGameObjects[i].GetComponent<Player>();

                playersGameObjects[i+1 ]= Instantiate(player, new Vector3(-1, -1, -1), Quaternion.identity);
                DontDestroyOnLoad(playersGameObjects[i]);
                playersGameObjects[i + 1].AddComponent<Player>();
                playerList[i+1] = playersGameObjects[i+1].GetComponent<Player>();
                playerList[i + 1].assignMesh();
                playerList[i + 1].ClearObject();
            }

        }
        if (!playerList[myNumber].isMain()) {
            for (int i = 0; i < 8; i++) {
                if (playerList[i].isMain())
                    myNumber = i;
            }
        }

        playersUpdated = true;

        
    }

    public void BackToLobby() {
        foreach (Player player in playerList) {
            player.BackToLobby();
        }
        gameMap = null;
        ended = false;
    }


    public void setMyNumber(int numb) {
        myNumber = numb;
        playerList[myNumber].setMain();
        playerList[myNumber].setName(MyName);
        playersUpdated = true;
    }
    public void addPlayeronNumber(int number, String name) {
        if (playerList[number].isEmpty()) {
            playerList[number].setName(name);
            playersUpdated = true;

        }
           
    }
    public void ClearAll() {
        myNumber = -1;
        MyName = "";
        for(int i =0; i <8; i++) {
            playerList[i].ClearObject();
        }
        
        playersUpdated = false;
    }

    public void removePlayerNumber(int number) {
        playerList[number].ClearObject();
        UpdatePlayers();
        
    }

    public void setPlayerReady(int numb, bool val) {
        playerList[numb].setReady(val);
        playersUpdated = true;
    }
}
