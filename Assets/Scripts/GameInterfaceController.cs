using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class GameInterfaceController : MonoBehaviour
{

    public GameObject LoadingScreen;
    public GameObject statsWindow;
    public GameObject gameEndWindow;
    public GameObject recWindow;
    public GameObject discWindow;

    //recWindow
    public Slider recStatus;

    //discWindow
    public Button discButton;

    //end window
    public Text endMessage;
    public Button oKButton;

    //Loading Screen
    public Slider loading;

    //MyStats
    public Text position;
    public Text highestScore;
    public Text myScore;
    public Text ping;
    public Text time;

    //All stats
    public Image[] Slot;
    public Text[] nick;
    public Text[] kills;
    public Text[] deaths;

    public void Start() {
        oKButton.onClick.AddListener(BackToLobby);
    }

    public void BackToLobby() {
        SceneManager.LoadScene("ConnectWindow", LoadSceneMode.Single);
    }

    public void ShowCantConnect() {
        recWindow.SetActive(false);
        discWindow.SetActive(true);
      
    }

    public void ShowRecconecting(bool val = true) {
        recWindow.SetActive(val);
    }

    public void updateRecconectStatus(float stat) {
        recStatus.value = stat;
    }

    public void ShowEndMessage(bool isLast = true, int num = -1, string name ="") {
        gameEndWindow.SetActive(true);
        if (isLast) {
            endMessage.text = "You're last player in game";
        }else {
            endMessage.text = "The game has ended, Player " + num.ToString() + " (" + name + ") won the game";
        }
    }

    public void UpdateTime(int min, int sec) {
        time.text = min.ToString() + ":";
        if (sec < 10)
            time.text += "0" + sec.ToString();
        else
            time.text += sec.ToString();
    }


    public void HideLoadingBox() {
        LoadingScreen.SetActive(false);
    }

    public void SetPositon(int pos) {
        position.text = "#" + pos.ToString();
    }

    public void setMyPoints(int points) {
        myScore.text = points.ToString();
    }

    public void setHighestScore(int score) {
        highestScore.text = score.ToString();
    }

    public void updateSlot(int number, Color color, string name = "", int score = -1, int ded = -1) {
        Slot[number].color = color;
        nick[number].text = name;
        if (score > -1) 
            kills[number].text = score.ToString();
        else
            kills[number].text = "";
        if (ded > -1)
            deaths[number].text = ded.ToString();
        else
            deaths[number].text = "";
    }

    public void updatePing(int _ping) {
        ping.text = _ping.ToString();
        if (_ping < 100)
            ping.color = Color.green;
        else if (_ping < 250)
            ping.color = Color.yellow;
        else
            ping.color = Color.red;

    }

    public void UpdateStage(float stage) {
        loading.value = stage;
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKey(KeyCode.Tab)) {
            statsWindow.SetActive(true);
        } else {
            statsWindow.SetActive(false);
        }
    }
}
