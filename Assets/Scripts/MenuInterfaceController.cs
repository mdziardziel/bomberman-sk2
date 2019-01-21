using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System;

public class MenuInterfaceController : MonoBehaviour
{
    //windows
    public GameObject connectWindow;
    public GameObject nicknameWindow;
    public GameObject lobbyWindow;
    public GameObject connectAlert;
    public GameObject reconectWindow;
    public GameObject disconectWindow;

    //ip connection input
    public Button connectButton;
    public InputField ipInput;
    public Text connectionMessages;

    //name input
    public Button nameButton;
    public InputField nameInput;
    public Text nameMessages;

    //lobby
    public Image[] lobbySlots;
    public Text[] playerNames;
    public Text[] playerPing;
    public Button readyButton;
    public Dropdown mapSize;
    public Dropdown gameDuration;

    //connecting menu
    public Text communicats;

    //disconnect menu
    public Button okButton;
    public Slider recStatus;


    public MenuController scMan;


    public enum box {ip,name,lobby,connect,none};

    // Start is called before the first frame update
    void Start()
    {
        scMan = GameObject.FindWithTag("SceneManager").GetComponent<MenuController>();
        connectButton.onClick.AddListener(delegate { scMan.InitializeConnection(ipInput.text); });
        nameButton.onClick.AddListener(delegate { scMan.InitializeSendName(nameInput.text); });
        readyButton.onClick.AddListener(scMan.InitializeReadySignal);
        ipInput.onValidateInput += delegate (string input, int charIndex, char addedChar) { return scMan.ipValidate(input,addedChar); };
        mapSize.onValueChanged.AddListener(delegate { scMan.InitializeMapSize(mapSize.value);});
        gameDuration.onValueChanged.AddListener(delegate { scMan.InitializeGameDuration(gameDuration.value); });

    }

    private void Update() {
        
        if(ipInput.isFocused &&  Input.GetKey(KeyCode.Return) && ipInput.text != "") {
            scMan.InitializeConnection(ipInput.text);
        } else if(nameInput.isFocused && Input.GetKey(KeyCode.Return) && nameInput.text != "") {
            scMan.InitializeSendName(nameInput.text);
        }
       
    }
    public void updateRecconectStatus(float stat) {
        recStatus.value = stat;
    }

    public void ShowRecconecting(bool val= true) {
        reconectWindow.SetActive(val);
    }

    public void ShowCantConnect() {
        reconectWindow.SetActive(false);
        disconectWindow.SetActive(true);
        okButton.onClick.AddListener(scMan.BackToBeggining);
    }

    public void ShowBox(box _box) {
        connectWindow.SetActive(false);
        nicknameWindow.SetActive(false);
        lobbyWindow.SetActive(false);
        connectAlert.SetActive(false);
        disconectWindow.SetActive(false);

        switch (_box) {
            case box.ip:
                connectWindow.SetActive(true);
                break;
            case box.name:
                nicknameWindow.SetActive(true);
                break;
            case box.lobby:
                lobbyWindow.SetActive(true);
                break;
            case box.connect:
                connectAlert.SetActive(true);
                break;
        }

    }

    public void ShowConnectionMessage(string message) {
        connectionMessages.text = message;
    }

    public void ShowNameMessage(string message,Color color) {
        nameMessages.text = message;
        nameMessages.color = color;
    }

    public void ToggleDropboxes(bool val = true) {
        mapSize.interactable = val;
        gameDuration.interactable = val;
    }

    public void ChangeLobbySlot(int playerNumber, string name, Color color) {
        lobbySlots[playerNumber].color = color;
        playerNames[playerNumber].text = name;
        playerPing[playerNumber].text = "";
    }

    public void WriteCommunicat(string message) {
        communicats.text = message;
    }



    public void UpdatePing(int playerNumber, float ping) {
        if (ping == -1)
            playerPing[playerNumber].text = "";
        else {
            playerPing[playerNumber].text = ping.ToString();
            if (ping < 100)
                playerPing[playerNumber].color = new Color(0, 1, 0);
            if (ping < 250)
                playerPing[playerNumber].color = new Color(1, 1, 0);
            else
                playerPing[playerNumber].color = new Color(1, 0, 0);
        }
    }
}
