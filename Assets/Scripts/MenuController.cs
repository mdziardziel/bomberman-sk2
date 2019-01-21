using System;
using System.Collections;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using UnityEngine.SceneManagement;
using UnityEngine;

public class MenuController : SceneController {
    ConnectionManager connection;
    MenuInterfaceController menuController;
    GlobalController gc;

    bool reconect;
    float recTime;

    // Start is called before the first frame update
    void Start() {
        connection = GameObject.FindWithTag("ConnectionManager").GetComponent<ConnectionManager>();
        menuController = GameObject.FindWithTag("MainMenuController").GetComponent<MenuInterfaceController>();
        gc = GameObject.FindWithTag("GlobalController").GetComponent<GlobalController>();
        if (connection.evenMoreConnected) {
            menuController.ShowBox(MenuInterfaceController.box.lobby);
            gc.BackToLobby();
            gc.playersUpdated = true;
        }
        connection.FindSceneManager();
    }

    public void InitializeSendName(string _name) {
        if (connection.SendName("N" + (_name.Length < 10 ? "0" + _name.Length.ToString() : _name.Length.ToString()) + _name)) {
            menuController.ShowBox(MenuInterfaceController.box.connect);     
            gc.MyName = _name;
        } else
            menuController.ShowNameMessage("Name already in use",Color.red);

    }

    public void InitializeConnection(string _ip) {
        Regex ipreg = new Regex(@"^\d{1,3}\.{1}\d{1,3}\.{1}\d{1,3}\.{1}\d{1,3}$");
        bool incorrect = false;
        if (ipreg.IsMatch(_ip)) {
            String[] ip = _ip.Split('.');
            foreach(String octet in ip) {
                if (int.Parse(octet) > 255) {               
                    menuController.ShowConnectionMessage("Incorrect IP address");
                    incorrect = true;
                }
            }
            if (!incorrect) {
                if (connection.MakeConnection(_ip, 1234)) {
                    menuController.ShowBox(MenuInterfaceController.box.name);
                    menuController.ShowNameMessage("Connected To Server",Color.green);
                } else
                    menuController.ShowConnectionMessage("Can't Connect To Server");
            }
        } else if(new Regex(@"^\d{1,3}\.{1}\d{1,3}\.{1}\d{1,3}\.{1}\d{1,3}\:\d{1,5}$").IsMatch(_ip)) {
            string[] data = _ip.Split(':');
            String[] ip = data[0].Split('.');
            foreach (String octet in ip) {
                if (int.Parse(octet) > 255) {
                    menuController.ShowConnectionMessage("Incorrect IP address");
                    incorrect = true;
                }
            }
            if (!incorrect) {
                if (connection.MakeConnection(data[0], int.Parse(data[1]))) {
                    menuController.ShowBox(MenuInterfaceController.box.name);
                    menuController.ShowNameMessage("Connected To Server",Color.green);
                } else
                    menuController.ShowConnectionMessage("Can't Connect To Server");
            }
        } else {
            menuController.ShowConnectionMessage("Incorrect IP address");
        }
    }

    public override void TryToRecconect() {
         reconect = true;
         recTime = 0.0f;
        menuController.ShowRecconecting();
    }

    internal void BackToBeggining() {
        gc.ClearAll();
        menuController.ShowBox(MenuInterfaceController.box.ip);
    }

    internal void InitializeGameDuration(int value) {
        connection.SendPacket("T" + ((3 + value * 2) < 10 ? "0" : "") + (3 + value * 2).ToString());
    }

    internal void InitializeMapSize(int value) {
        connection.SendPacket("F" + (11 + value * 10).ToString() + (11 + value * 10).ToString());
    }

    public void InitializeReadySignal() {
        if (gc.playerList[gc.myNumber].isReady()) {
            connection.SendPacket("G0");
            gc.setPlayerReady(gc.myNumber, false);
        } else {
            connection.SendPacket("G1");
            gc.setPlayerReady(gc.myNumber, true);
        }
    }

    public void CalculateLastPing(int ping) {
        if (gc.myNumber != -1)
            try {
                menuController.UpdatePing(gc.myNumber, (int)(connection.checkPing(ping) * 100));
            }catch {
                //
            }
        else {
            connection.checkPing(ping);
        }
    }



    public override void DecodeMessage(string message) {
        if (message != "") {

            if (new Regex(@"^\d{1}$").IsMatch(message)) {
                CalculateLastPing(int.Parse(message));
            } else if(connection.evenMoreConnected){
                switch (message[0]) {
                    case 'O': //succesfully connected

                        break;
                    case 'Q': //player number XX number
                        gc.setMyNumber(int.Parse(message[1].ToString()));
                        menuController.WriteCommunicat("Getting my number " + message);
                        break;
                    case 'N': //other player XX number XX length
                        gc.addPlayeronNumber(int.Parse(message[1].ToString()), message.Substring(4, int.Parse(message.Substring(2, 2))));
                        menuController.WriteCommunicat("Getting name of player nr " + message[1] + " " + message.Substring(4, int.Parse(message.Substring(2, 2))));
                        break;
                    case 'S': //start of the game
                        connection.ToggleGame(true);
                        SceneManager.LoadScene("BombermanGame", LoadSceneMode.Single);
                        break;
                    case 'X': //game end

                        break;
                    case 'R': //player leaved XX
                        if (int.Parse(message[1].ToString()) != gc.myNumber) {
                            gc.removePlayerNumber(int.Parse(message[1].ToString()));
                            ClearPing();
                        }
                        break;
                    case 'G':
                        gc.setPlayerReady(int.Parse(message[1].ToString()), message[2] == '1' ? true : false);
                        break;
                }
            }
        }
    }

    public void ClearPing() {
        for (int i = 0; i < 8; i++)
            menuController.UpdatePing(i, -1);
    }

    private void LateUpdate() {
        if (gc.playersUpdated) {
            for( int i =0; i < 8; i++) {
                if (!gc.playerList[i].isEmpty()) {
                    if (gc.playerList[i].isReady()) {
                        menuController.ChangeLobbySlot(i, gc.playerList[i].getName(), new Color(0f, 1f, 0f));
                        menuController.playerNames[i].color = new Color(0, 0, 0);
                    } else {
                        menuController.ChangeLobbySlot(i, gc.playerList[i].getName(), new Color(0.2f, 0.2f, 0.7f));
                        menuController.playerNames[i].color = new Color(1, 1, 1);
                    }
                } else
                    menuController.ChangeLobbySlot(i, "", new Color(0.2f, 0.2f, 0.2f));
            }
            gc.playersUpdated = false;
        }
        if(gc.myNumber == 0) {
            menuController.ToggleDropboxes();
        }

        if (gc.myNumber != -1 && menuController.connectAlert.activeSelf) {
            Debug.Log(menuController.connectAlert.activeSelf);
            bool allin = true;
            for (int i =0; i <gc.myNumber; i++) {
                if (gc.playerList[i].isEmpty())
                    allin = false;
            }
            if (allin)
                menuController.ShowBox(MenuInterfaceController.box.lobby);

        }

        if (reconect) {
            if (recTime < 10.0f) {
                if (connection.MakeConnection(connection.ipAddres, connection.globlaPort)) {
                    if (gc.MyName != "") {
                        connection.SendPacket("O");
                        connection.SendPacket("N" + gc.MyName.Length + gc.MyName);
                        connection.evenMoreConnected = true;
                    }
                    reconect = false;
                    menuController.ShowRecconecting(false);
                }
                recTime += Time.deltaTime;
                menuController.updateRecconectStatus(recTime);
            } else {
                menuController.ShowCantConnect();
                reconect = false;
            }
        }
    }

    public char ipValidate(string _input, char _char) {
        Regex numb = new Regex(@"\d{1}");

        if (_char != '.' && _char != ':' && !numb.IsMatch(_char.ToString())) {
            _char = '\0';
        }
        Regex dotdot = new Regex(@"^*\.{1}$");
        Regex colcol = new Regex(@"^*\:{1}$");
        if (dotdot.IsMatch(_input) && _char == '.') {
            _char = '\0';
        }

        if (colcol.IsMatch(_input) && _char == ':') {
            _char = '\0';
        }

        Regex first3 = new Regex(@"^\d{3}$");
        if (first3.IsMatch(_input) && _char != '.') {
            _char = '\0';
        }
        Regex second3 = new Regex(@"^\d{1,3}\.{1}\d{3}$");
        if (second3.IsMatch(_input) && _char != '.') {
            _char = '\0';
        }
        Regex third3 = new Regex(@"^\d{1,3}\.{1}\d{1,3}\.{1}\d{3}$");
        if (third3.IsMatch(_input) && _char != '.') {
            _char = '\0';
        }
        Regex last3 = new Regex(@"^\d{1,3}\.{1}\d{1,3}\.{1}\d{1,3}\.{1}\d{3}$");
        if (last3.IsMatch(_input) && _char != ';') {
            _char = '\0';
        }
        Regex ip = new Regex(@"^\d{1,3}\.{1}\d{1,3}\.{1}\d{1,3}\.{1}\d{1,3}$");
        if (!ip.IsMatch(_input) && _char == ':') {
            _char = '\0';
        }

        Regex port = new Regex(@"^\d{1,3}\.{1}\d{1,3}\.{1}\d{1,3}\.{1}\d{1,3}\:{1}\d{5}");
        if (port.IsMatch(_input)) {
            _char = '\0';
        }
        

        return _char;
    }



}
