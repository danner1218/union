<?php
class DB
{
    public static $instances = array();

    public static function link($conf, $relink = False) {
        if(is_string($conf)) {
            $db_key = $conf;
            $conf   = Core::config("db.{$conf}");
        } else {
            $db_key = md5((string)$conf);
        }
        if(!isset(self::$instances[$db_key]) || $relink == True) {
            self::$instances[$db_key] = new CDataBase($conf);
        }
        return self::$instances[$db_key];
    }
}
class CDataBase
{
    public $connection = null;
    private $stmt = null;
    public function __construct($config)
    {
        $this->connection = new PDO($config['dsn'], $config['username'], $config['password'], $config['options']);
    }
    public function insert($tablename, $params)
    {
        $fields = array_keys($params);
        $field = implode(',', $fields);
        $question = str_repeat('?, ', count($params));
        $question = rtrim($question, ', ');
        $sql = sprintf('INSERT INTO %s (%s) VALUES (%s)',
            $tablename,
            $field,
            $question);
        $result = $this->query($sql, $params);
        return $result;
        //return $this->lastInsertId();
    }
    public function update($tablename, $params, $where = 1)
    {
        $fields = array_keys($params);
        $field = implode(' = ?, ', $fields).' = ?';
        $sql = sprintf('UPDATE %s SET %s WHERE %s',
            $tablename,
            $field,
            $where);
        $result = $this->query($sql, $params);
        return $this->rowCount();
    }
    public function delete($tablename, $where)
    {
        $sql = '';
        $result = $this->query($sql, $params);
        return $this->rowCount();
    }
    public function fetch($sql, $params = array(), $fetchMode = PDO::FETCH_ASSOC)
    {
        $this->query($sql, $params);
        return $this->stmt->fetch($fetchMode);
    }
    public function fetchAll($sql, $params = array(), $fetchMode = PDO::FETCH_ASSOC)
    {
        $queryId = $this->query($sql, $params);
        return $this->stmt->fetchAll($fetchMode);
    }
    public function lastInsertId()
    {
        return $this->connection->lastInsertId();
    }
    public function rowCount()
    {
        return $this->stmt->rowCount();
    }
    public function query($sql, $params = array())
    {
        $this->_prepare($sql);
        $key = 1;
        foreach($params as $param)
        {
            $this->_bindParam($key++, $param);
        }
        return $this->_execute();
    }
    private function _execute()
    {
        $result = $this->stmt->execute();
        return $result;
    }
    private function _prepare($sql)
    {
        $this->stmt = $this->connection->prepare($sql);
    }
    private function _bindParam($key, $param)
    {
        $this->stmt->bindParam($key, $param);
    }
}
