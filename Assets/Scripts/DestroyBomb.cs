using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DestroyBomb : MonoBehaviour {

    public bool isBomb;
    public int owningPlayer;
    public float timeToDestroy;
    public GameObject explosion;

	// Use this for initialization
	void Start () {
        Destroy(gameObject, timeToDestroy);

	}

    private void OnDestroy() {
        if (isBomb) {
           GameObject exp = Instantiate(explosion, transform.position, Quaternion.identity);
            exp.GetComponent<DestroyOnHardWall>().owningPlayer = owningPlayer;
        }
    }
}
