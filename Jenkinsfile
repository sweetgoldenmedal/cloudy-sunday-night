pipeline {
  agent any
  stages {
    stage('Build') {
			agent { label 'craablab' }
      steps {
				sh 'hostname'
				sh 'ls code.py'
				sh 'sudo cp code.py /mnt/code/py'
        //sh 'echo "Hello World"'
        /*sh '''
                    echo "Multiline shell steps works too"
                    ls -lah
                '''
				*/
      }
    }
  }
}
