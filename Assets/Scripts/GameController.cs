using System;
using System.Collections;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using UnityEngine;

public class GameController : SceneController {

    public ConnectionManager cm;
    public GlobalController gc;
    public GameInterfaceController gameInterface;
    public float stage = 0;
    public float gameTime;
    public GameObject stillWall;
    public GameObject box;
    public GameObject walls;
    public Transform mapContainer;
    public bool reconect;
    float recTime;
    int[,] Walls;

    public float[] pingList = new float[10];


    public override void TryToRecconect() {
        gameInterface.ShowRecconecting();
        reconect = true;
        recTime = 0.0f;

    }

    // Use this for initialization
    void Awake() {
        gc = GameObject.FindWithTag("GlobalController").GetComponent<GlobalController>();
        cm = GameObject.FindWithTag("ConnectionManager").GetComponent<ConnectionManager>();
        cm.FindSceneManager();
    }

    public void SendMyMove(int x, int y) {
        cm.SendPacket("P" + (x < 10? "0"+x.ToString() : x.ToString() ) + (y < 10 ? "0" + y.ToString() : y.ToString()));
    }

    public void SendTheBombHasBeenPlanted(int x, int y) {
        cm.SendPacket("B" + (x < 10 ? "0" + x.ToString() : x.ToString()) + (y < 10 ? "0" + y.ToString() : y.ToString()));
    }

    protected void InitializeMap() {
        mapContainer.localScale = new Vector3(((float)(gc.gameMap.getSize('x')+1))/10.0f, 1, ((float)(gc.gameMap.getSize('y')+1))/10.0f);
        mapContainer.position += new Vector3((gc.gameMap.getSize('x')-10) / 2, 0, (gc.gameMap.getSize('y')-10) / 2);
        for (int x = 0; x < gc.gameMap.getSize('x'); x++)
            for (int y = 0; y < gc.gameMap.getSize('y'); y++) {
                if (x % 2 == 1 && y % 2 == 1) {
                   GameObject wall = Instantiate(stillWall, new Vector3(x, 0, y), Quaternion.identity);
                    wall.transform.SetParent(walls.transform);
                }
            }
        stage =1;
        gameInterface.UpdateStage(stage);
        mapMeshCombine();
    }

    protected void mapMeshCombine() {
        MeshFilter[] meshFilters = walls.gameObject.GetComponentsInChildren<MeshFilter>();
        CombineInstance[] combine = new CombineInstance[meshFilters.Length];
        for (int i = 0; i < meshFilters.Length; i++) {
            combine[i].mesh = meshFilters[i].sharedMesh;
            combine[i].transform = meshFilters[i].transform.localToWorldMatrix;
            meshFilters[i].gameObject.SetActive(false);
        }
        walls.GetComponent<MeshFilter>().mesh = new Mesh();
        walls.GetComponent<MeshFilter>().mesh.CombineMeshes(combine);
        walls.transform.position = Vector3.zero;
        walls.SetActive(true);


    }

    protected void PopulateMapWithBoxes(string message) {
        int actual = 1;
        for (int i = 0; i <= gc.gameMap.getSize('x'); i++)
            for (int j = 0; j <= gc.gameMap.getSize('y'); j++) {
                if (message[actual] == '1')
                    gc.gameMap.AddBox(i, j,box);
                actual++;
            }
        stage =2;
        gameInterface.UpdateStage(stage);
    }

    public void CalculateLastPing(int ping) {
        gameInterface.updatePing((int)(cm.checkPing(ping)*100));
    }

    public override void DecodeMessage(string message) {
        if (message != "") {

            if (new Regex(@"^\d{1}$").IsMatch(message)) {
                CalculateLastPing(int.Parse(message));
            } else {

                switch (message[0]) {
                    case 'P': // Z player moved to XXYY position PZXXYY
                        if (gc.playerList[int.Parse(message[1].ToString())].isPositioned())
                            gc.playerList[int.Parse(message[1].ToString())].movePlayer(int.Parse(message.Substring(2, 2)), int.Parse(message.Substring(4, 2)));
                        else {
                            gc.playerList[int.Parse(message[1].ToString())].setPosition(int.Parse(message.Substring(2, 2)), int.Parse(message.Substring(4, 2)));
                            CheckStage(int.Parse(message[1].ToString()));
                        }
                        break;
                    case 'B': // put bomb of Z player in XY position BZXXYY
                        gc.PlaceBombInPoint(int.Parse(message.Substring(1, 1)), int.Parse(message.Substring(2, 2)), int.Parse(message.Substring(4, 2)));
                        break;
                    case 'K':// player X died KX
                        gc.playerList[int.Parse(message[1].ToString())].setDead(true);
                        break;
                    case 'A': // X player have YY points AXYY
                        gc.playerList[int.Parse(message[1].ToString())].setPoints(int.Parse(message.Substring(2,2)));
                        UpdatePoints();
                        break;
                    case 'T': // format XXX.XXX example T482.234 in seconds
                        gameTime = float.Parse(message.Substring(1, 7));
                        break;
                    case 'N': //player X joined YY name lenght then name NXYYname
                        gc.addPlayeronNumber(int.Parse(message[1].ToString()), message.Substring(4, int.Parse(message.Substring(2, 2))));
                        break;
                    case 'R': //player X disconnested
                        gc.removePlayerNumber(int.Parse(message[1].ToString()));
                        UpdatePoints();
                        break;
                    case 'F': // map resolution FXXYY
                        Debug.Log("gettedThis");
                        gc.gameMap = new Map();
                        gc.gameMap.Initialize(int.Parse(message.Substring(1, 2)), int.Parse(message.Substring(3, 2)));
                        InitializeMap();
                        break;
                        
                    case 'M': // map in matrix format; MXXXXX..... {0, 1} ; 0- empty ; 1 - box
                        PopulateMapWithBoxes(message);     
                        break;
                    case 'G':
                        gc.setPlayerReady(int.Parse(message[1].ToString()), message[2] == '1' ? true : false);
                        break;
                    case 'X': // game ended, Y - why game ende {0,1} ; 0 - only 1 player remained; 1 - one player won; if 1, then additional number of player who won  X0/ X1Y
                        if (message[1] == '0')
                            gameInterface.ShowEndMessage();
                        else
                            gameInterface.ShowEndMessage(false,int.Parse(message[2].ToString()),gc.playerList[int.Parse(message[2].ToString())].getName());
                        cm.ToggleGame(false);
                        gc.ended = true;
                        break;
                    
                }

                
            }
        }
    }

    private void Update() {
        if (reconect) {
            if (recTime < 10.0f) {
                if (cm.MakeConnection(cm.ipAddres, cm.globlaPort)) {
                    cm.SendPacket("O");
                    cm.SendPacket("N" + gc.MyName.Length + gc.MyName);
                    cm.evenMoreConnected = true;
                    reconect = false;
                    gameInterface.ShowRecconecting(false);
                }
                recTime += Time.deltaTime;
                gameInterface.updateRecconectStatus(recTime % 5);
            } else {
                gameInterface.ShowCantConnect();
                reconect = false;
            }
        }
        if (!gc.ended) {
            gameTime -= Time.deltaTime;

            int min = (int)Mathf.Floor(gameTime / 60);
            int sec = (int)(gameTime) % 60;
            gameInterface.UpdateTime(min, sec);
        }

    }

    private void UpdatePoints() {
        int most = -1;
        int my= -1;
        for(int i =0; i <8; i++) {
            if (!gc.playerList[i].isEmpty()) {
                gameInterface.updateSlot(i, new Color(0.2f, 0.8f, 0.2f, 0.5f), gc.playerList[i].getName(), gc.playerList[i].getPoints());
                if (gc.playerList[i].getPoints() > most) most = gc.playerList[i].getPoints();
                if(gc.playerList[i].isMain()) my = gc.playerList[i].getPoints();
            }else {
                gameInterface.updateSlot(i, new Color(0.2f, 0.2f, 0.2f, 0.5f));
            }
        }
        int pos = 1;
        foreach(Player player in gc.playerList) {
            if (player.getPoints() > my)
                pos++;

        }
        gameInterface.setHighestScore(most);
        gameInterface.setMyPoints(my);
        gameInterface.SetPositon(pos);  
    }

    public void CheckStage(int numb) {
        if(stage >= 2 && stage < 3) {
            bool allin = true;
            stage = 2;
            foreach (Player player in gc.playerList) {
                if (!player.isEmpty()) {
                    if (!player.isPositioned())
                        allin = false;
                    else
                        stage += 0.124f;
                }
            }
            gameInterface.UpdateStage(stage);
            if (allin) {

                foreach (Player player in gc.playerList) {
                    if (!player.isEmpty()) {
                        player.setDead(false);
                        player.InstantiateObject();
                    }
                }
                UpdatePoints();
                gameInterface.HideLoadingBox();
                stage = 3;
                gameInterface.UpdateStage(stage);
            }
        } else if (stage == 3) {
            gc.playerList[numb].setDead(false);
            UpdatePoints();
        }
    }


}
